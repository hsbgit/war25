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
#include <cmath>
#include <cassert>
#include <algorithm>
#include <chrono>
#include <iostream>

#ifdef _DEBUG
std::unordered_set<INode*> g_debugSetOpenList;
std::unordered_set<INode*> g_debugSetClosedList;
Path g_debugPath;
#endif


AStar::AStar(INode* pStartNode, INode* pEndNode, int unitSize, bool airPath) : m_pEndNode(pEndNode), m_unitSize(unitSize), m_airPath(airPath) {
	// ToDo: call/implement for air / land units
	NeighbourNodes vecNeighourNodes = pStartNode->getFreeNeighbourNodes(m_unitSize, m_airPath);

	for (auto& [direction, pNode] : vecNeighourNodes) {
		pNode->m_pParentNode = pStartNode;
		pNode->m_g = (direction == Direction::NW || direction == Direction::NE || direction == Direction::SW || direction == Direction::SE) ? 14 : 10; // rename?   
		pNode->m_h = heuristic(pNode);

		m_setOpenList.insert(pNode);
		m_mapOpenList_f_values[pNode->getF()].insert(pNode);
	}

	pStartNode->m_pParentNode = nullptr;
	m_setClosedList.insert(pStartNode);
}


int AStar::heuristic(const INode* pNode) {
	return 10 * (std::abs(pNode->getX() - m_pEndNode->getX()) + std::abs(pNode->getY() - m_pEndNode->getY()));
}


Path AStar::findPath() {
#ifdef _DEBUG
	auto start = std::chrono::high_resolution_clock::now();
#endif

	Path path;

	while (m_setOpenList.size()) {
		INode* pNode_fMin = getNodeWith_fMin();
		NeighbourNodes neighourNodes = pNode_fMin->getFreeNeighbourNodes(m_unitSize, m_airPath);

		// For each of the squares adjacent to this current square:
		for (auto& [direction, pNode] : neighourNodes) {
			if (m_setClosedList.contains(pNode)) {
				// If it is on the closed list, ignore it.
				continue;
			}

			int costs = (direction == Direction::NW || direction == Direction::NE || direction == Direction::SW || direction == Direction::SE) ? 14 : 10; // rename?            

			if (m_setOpenList.contains(pNode)) {
				// If an adjacent square is already on the open list, check to see if this path to
				// that square is a better one. In other words, check to see if the G score for that
				// square is lower if we use the current square to get there. If not, don’t do anything.
				int possibleNewG = pNode_fMin->m_g + costs;

				if (possibleNewG < pNode->m_g) {
					// Since we change g the sum f will most likely also be changed
					int old_f = pNode->getF();

					pNode->m_g = possibleNewG;
					pNode->m_pParentNode = pNode_fMin;

#ifdef _DEBUG
					if (pNode == pNode_fMin->m_pParentNode && pNode_fMin == pNode->m_pParentNode) {
						std::cerr << "Got a problem" << std::endl;
					}
#endif

					if (old_f != pNode->getF()) {
						// f changed indeed. Hence we remove this node from its old position within our f map
						std::unordered_set<INode*>& setNodes = m_mapOpenList_f_values[old_f];

						assert(setNodes.contains(pNode));
						setNodes.erase(pNode);

						if (!setNodes.size()) {
							m_mapOpenList_f_values.erase(old_f);
						}

						// And add it back to its new position
						m_mapOpenList_f_values[pNode->getF()].insert(pNode);
					}
				}
			}
			else {
				pNode->m_pParentNode = pNode_fMin;
				pNode->m_g = pNode_fMin->m_g + costs;
				pNode->m_h = heuristic(pNode);

#ifdef _DEBUG
				if (pNode == pNode_fMin->m_pParentNode && pNode_fMin == pNode->m_pParentNode) {
					std::cerr << "Got a problem" << std::endl;
				}
#endif

				m_setOpenList.insert(pNode);
				m_mapOpenList_f_values[pNode->getF()].insert(pNode);
			}
		}

		// Stop when you: Add the target square to the closed list, in which case the path has been found (see note below), or
		if (m_setClosedList.contains(m_pEndNode)) {
			path.m_vecPathNodes = buildFinalPath(m_pEndNode);
			break;
		}

		if (!m_setOpenList.size()) {
			std::cerr << "[Pathfinding] No path found!" << std::endl;
			// ToDo: return closest tile?
			// sth. like path.m_vecPathNodes = buildFinalPath(pNode_fMin);  
			break;
		}
	}

#ifdef _DEBUG
	auto end = std::chrono::high_resolution_clock::now();
	float diff = (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	g_debugSetClosedList = m_setClosedList;
	g_debugSetOpenList = m_setOpenList;
	g_debugPath = path;

	std::cerr << "[Pathfinding] Required time to find path: " << diff << "[ms]" << std::endl;
#endif

	return path;
}


std::vector<INode*> AStar::buildFinalPath(INode* pNode) {
	std::vector<INode*> vecPath = { pNode };  // Could be a set as a path should contain each tile just once. But as a set is not ordered sequentially the path would get destroyed. In Addition the following std::reverse is not possible on a set

	// Go back from end node to start node
	INode* pParent = pNode->m_pParentNode;

	// Bug: Endless loop? Appeared when moving a lot of units
	while (pParent) {
		if (std::find(vecPath.begin(), vecPath.end(), pParent) != vecPath.end()) {
			std::cerr << "Got a problem" << std::endl;
			break;
		}

		vecPath.push_back(pParent);
		pParent = pParent->m_pParentNode;
	}

	// Remove start point ( where the unit is currently standing)
	vecPath.pop_back();

	// Reverse, to get the right movement ordering from start to end node
	std::reverse(vecPath.begin(), vecPath.end());

	// Clear parent nodes for next AStar runs
	for (auto& pNode : m_setOpenList) {
		pNode->m_pParentNode = nullptr;
	}

	for (auto& pNode : m_setClosedList) {
		pNode->m_pParentNode = nullptr;
	}

	return vecPath;
}


INode* AStar::getNodeWith_fMin() {
	// while loop which is calling 'getNodeWith_fMin' should have been left if openlist is empty!
	assert(m_mapOpenList_f_values.size());

	std::unordered_set<INode*>& setNodes = m_mapOpenList_f_values.begin()->second;

	// Map keys are always sorted unless std::unordered_map is used
	INode* pNode = *setNodes.begin();

	// Remove from open list and from f list
	setNodes.erase(setNodes.begin());

	if (!setNodes.size()) {
		m_mapOpenList_f_values.erase(m_mapOpenList_f_values.begin());
	}

	m_setOpenList.erase(pNode);
	m_setClosedList.insert(pNode);
	return pNode;
}
