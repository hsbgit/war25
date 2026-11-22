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
#include "Map/MovementRules.h"
#include "Map/PathSearch.h"
#include "Units/Unit.h"

#include <algorithm>
#include <cmath>

std::unordered_set<INode*> g_debugSetOpenList;
std::unordered_set<INode*> g_debugSetClosedList;
Path g_debugPath;

namespace {
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
				if (!MovementRules::terrainAllows(m_pMovingObject->isLandObject(),
					m_pMovingObject->isSeaObject(), m_pMovingObject->canDockOnCoast(),
					tile->isLand(), tile->isSea(), tile->isCoast(), tile->isRocks(),
					tile->isWood(), tile->isWall())) {
					return false;
				}
			}
		}
		return true;
	};
	if (!footprintFitsTerrain(to->getX(), to->getY()))
		return false;

	// Keep the existing swept-footprint behavior for air units. Land and sea
	// movement use the classic eight-way grid and only require the destination
	// footprint to fit.
	if (MovementRules::requiresOrthogonalDiagonalClearance(m_airPath) &&
		from->getX() != to->getX() && from->getY() != to->getY()) {
		if (!footprintFitsTerrain(to->getX(), from->getY()) ||
			!footprintFitsTerrain(from->getX(), to->getY()))
			return false;
	}
	return true;
}

std::optional<int> AStar::occupancyCost(const INode* node,
	bool allowTemporaryUnits) const {
	if (!m_pMovingObject)
		return 0;
	Map* map = m_pMovingObject->getMap();
	int cost = 0;
	for (int y = node->getY(); y < node->getY() + m_unitSize; ++y) {
		for (int x = node->getX(); x < node->getX() + m_unitSize; ++x) {
			Tile* tile = map->getTile({x, y});
			Object* occupant = m_airPath ? tile->getAirUnit() : tile->getSeaOrLandUnit();
			const auto tileCost = MovementRules::occupancyTraversalCost(
				occupant != nullptr, occupant == m_pMovingObject,
				dynamic_cast<Unit*>(occupant) != nullptr,
				allowTemporaryUnits);
			if (!tileCost)
				return std::nullopt;
			cost = std::max(cost, *tileCost);
		}
	}
	return cost;
}

Path AStar::findPath() {
	Path result;
	if (!m_pStartNode || !m_pEndNode || m_pStartNode == m_pEndNode)
		return result;

	auto neighbourValues = [this](INode* current) {
		if (m_pMovingObject) {
			Tile* tile = static_cast<Tile*>(current);
			return (m_airPath || m_pMovingObject->canDockOnCoast())
				? tile->neighbourTiles : tile->neighbourTiles_SameType;
		}
		return current->getFreeNeighbourNodes(m_unitSize, m_airPath);
	};
	auto runSearch = [this, &neighbourValues](bool allowTemporaryUnits,
		bool returnClosestReachable) {
		return PathSearch::find(m_pStartNode, m_pEndNode, neighbourValues,
			[this, allowTemporaryUnits](INode* from, INode* to) {
				return canVisit(from, to) &&
					occupancyCost(to, allowTemporaryUnits).has_value();
			},
			[this, allowTemporaryUnits](Direction direction, INode* node) {
				return (isDiagonal(direction) ? 14 : 10) +
					occupancyCost(node, allowTemporaryUnits).value_or(0);
			},
			[this](INode* node) { return heuristic(node); },
			returnClosestReachable);
	};

	PathSearch::Result<INode> search;
	if (m_pMovingObject && m_pMovingObject->isLandObject()) {
		// Prefer every free detour, regardless of its length. Only if no such
		// route exists may A* plan through a temporarily occupied chokepoint.
		search = runSearch(false, false);
		if (!search.reachedGoal)
			search = runSearch(true, false);
		// A clicked tree, wall or building tile cannot be entered. Return the
		// closest actually reachable land tile instead of abandoning the order.
		if (!search.reachedGoal)
			search = runSearch(false, true);
	} else {
		// Do not alter the established ship and air-unit routing behavior.
		search = runSearch(true, false);
	}
	result.m_vecPathNodes = std::move(search.path);
	m_setOpenList = std::move(search.open);
	m_setClosedList = std::move(search.closed);

	g_debugSetClosedList = m_setClosedList;
	g_debugSetOpenList = m_setOpenList;
	g_debugPath = result;
	return result;
}
