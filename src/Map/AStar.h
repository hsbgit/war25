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
#pragma once

#include <vector>
#include <unordered_set>

#include "Path.h"

class INode;
class Object;

#ifdef _DEBUG
extern std::unordered_set<INode*> g_debugSetOpenList;
extern std::unordered_set<INode*> g_debugSetClosedList;
extern Path g_debugPath;
#endif



class AStar {
public:
	AStar(INode* pStartNode, INode* pEndNode, int unitSize = 1,
		bool airPath = false, Object* movingObject = nullptr);

	Path findPath();

	inline const std::unordered_set<INode*>& debug_openList() const {
		return m_setOpenList;
	}

	inline const std::unordered_set<INode*>& debug_closedList() const {
		return m_setClosedList;
	}

private:
	int heuristic(const INode* pNode) const;
	bool canVisit(const INode* from, const INode* to) const;
	bool isDynamicallyBlocked(const INode* node) const;

	INode* m_pStartNode;
	INode* m_pEndNode;
	int m_unitSize;
	bool m_airPath;
	Object* m_pMovingObject;

	std::unordered_set<INode*> m_setOpenList;
	std::unordered_set<INode*> m_setClosedList;
};
