/*
	Copyright (C) 2024 P. Last

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include "AStar.h"
#include "INode.h"
#include "Map/Map.h"
#include "Map/Tile.h"
#include "Global/Object.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

#ifdef _DEBUG
std::unordered_set<INode*> g_debugSetOpenList;
std::unordered_set<INode*> g_debugSetClosedList;
Path g_debugPath;
#endif

namespace {
struct QueueEntry {
	int f;
	int h;
	INode* node;
};

struct LowestScoreFirst {
	bool operator()(const QueueEntry& lhs, const QueueEntry& rhs) const {
		if (lhs.f != rhs.f) return lhs.f > rhs.f;
		if (lhs.h != rhs.h) return lhs.h > rhs.h;
		if (lhs.node->getY() != rhs.node->getY())
			return lhs.node->getY() > rhs.node->getY();
		return lhs.node->getX() > rhs.node->getX();
	}
};

bool isDiagonal(Direction direction) {
	return direction == Direction::NW || direction == Direction::NE ||
		direction == Direction::SW || direction == Direction::SE;
}
}

AStar::AStar(INode* start, INode* end, int unitSize, bool airPath,
	Object* movingObject)
	: m_pStartNode(start), m_pEndNode(end), m_unitSize(unitSize),
	  m_airPath(airPath), m_pMovingObject(movingObject) {}

int AStar::heuristic(const INode* node) const {
	// Octile distance is admissible for the 10/14 movement costs and explores
	// substantially fewer tiles than the old Manhattan estimate with diagonals.
	const int dx = std::abs(node->getX() - m_pEndNode->getX());
	const int dy = std::abs(node->getY() - m_pEndNode->getY());
	return 10 * std::max(dx, dy) + 4 * std::min(dx, dy);
}

bool AStar::canVisit(const INode* from, const INode* to) const {
	if (!m_pMovingObject)
		return true;
	Map* map = m_pMovingObject->getMap();
	auto footprintFitsTerrain = [this, map](int topLeftX, int topLeftY) {
		if (topLeftX < 0 || topLeftY < 0 ||
			topLeftX + m_unitSize > map->sideLength() ||
			topLeftY + m_unitSize > map->sideLength())
			return false;
		if (m_airPath)
			return true;
		for (int y = topLeftY; y < topLeftY + m_unitSize; ++y) {
			for (int x = topLeftX; x < topLeftX + m_unitSize; ++x) {
				const Tile* tile = map->getTile({x, y});
				if (m_pMovingObject->isLandObject()) {
					if (!tile->isLand() || tile->isCoast() || tile->isRocks() ||
						tile->isWood() || tile->isWall())
						return false;
				} else if (m_pMovingObject->isSeaObject() &&
					(!tile->isSea() || tile->isCoast())) {
					return false;
				}
			}
		}
		return true;
	};
	if (!footprintFitsTerrain(to->getX(), to->getY()))
		return false;

	// Check both orthogonal positions for diagonal movement. This is essential
	// for 2x2 hulls and also prevents 1x1 land units cutting solid corners.
	if (from->getX() != to->getX() && from->getY() != to->getY()) {
		if (!footprintFitsTerrain(to->getX(), from->getY()) ||
			!footprintFitsTerrain(from->getX(), to->getY()))
			return false;
	}
	return true;
}

bool AStar::isDynamicallyBlocked(const INode* node) const {
	if (!m_pMovingObject)
		return false;
	Map* map = m_pMovingObject->getMap();
	for (int y = node->getY(); y < node->getY() + m_unitSize; ++y) {
		for (int x = node->getX(); x < node->getX() + m_unitSize; ++x) {
			Tile* tile = map->getTile({x, y});
			Object* occupant = m_airPath ? tile->getAirUnit() : tile->getSeaOrLandUnit();
			if (occupant && occupant != m_pMovingObject)
				return true;
		}
	}
	return false;
}

Path AStar::findPath() {
	Path result;
	if (!m_pStartNode || !m_pEndNode || m_pStartNode == m_pEndNode)
		return result;

	std::priority_queue<QueueEntry, std::vector<QueueEntry>, LowestScoreFirst> open;
	std::unordered_map<INode*, int> scores;
	std::unordered_map<INode*, INode*> parents;
	scores[m_pStartNode] = 0;
	open.push({heuristic(m_pStartNode), heuristic(m_pStartNode), m_pStartNode});
	m_setOpenList.insert(m_pStartNode);

	while (!open.empty()) {
		const QueueEntry entry = open.top();
		open.pop();
		INode* current = entry.node;
		if (m_setClosedList.contains(current))
			continue; // stale priority-queue entry
		m_setOpenList.erase(current);
		m_setClosedList.insert(current);

		if (current == m_pEndNode) {
			std::vector<INode*> nodes;
			for (INode* node = current; node != m_pStartNode; node = parents.at(node))
				nodes.push_back(node);
			std::reverse(nodes.begin(), nodes.end());
			result.m_vecPathNodes = std::move(nodes);
			break;
		}

		NeighbourNodes availableNeighbours;
		const NeighbourNodes* neighbours;
		if (m_pMovingObject) {
			Tile* tile = static_cast<Tile*>(current);
			neighbours = m_airPath ? &tile->neighbourTiles : &tile->neighbourTiles_SameType;
		} else {
			availableNeighbours = current->getFreeNeighbourNodes(m_unitSize, m_airPath);
			neighbours = &availableNeighbours;
		}
		for (const auto& [direction, neighbour] : *neighbours) {
			if (m_setClosedList.contains(neighbour) || !canVisit(current, neighbour))
				continue;
			// Units are temporary obstacles. A finite penalty prefers a free detour,
			// while still producing a route through a moving crowd or a chokepoint.
			const int congestionPenalty = isDynamicallyBlocked(neighbour) ? 40 : 0;
			const int tentative = scores[current] +
				(isDiagonal(direction) ? 14 : 10) + congestionPenalty;
			auto old = scores.find(neighbour);
			if (old != scores.end() && tentative >= old->second)
				continue;
			scores[neighbour] = tentative;
			parents[neighbour] = current;
			const int h = heuristic(neighbour);
			open.push({tentative + h, h, neighbour});
			m_setOpenList.insert(neighbour);
		}
	}

#ifdef _DEBUG
	g_debugSetClosedList = m_setClosedList;
	g_debugSetOpenList = m_setOpenList;
	g_debugPath = result;
#endif
	return result;
}
