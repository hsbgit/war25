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

#include <map>
#include <vector>
#include <unordered_set>

#include "Path.h"

class INode;

#ifdef _DEBUG
extern std::unordered_set<INode*> g_debugSetOpenList;
extern std::unordered_set<INode*> g_debugSetClosedList;
extern Path g_debugPath;
#endif



class AStar {
public:
	AStar(INode* pStartNode, INode* pEndNode, int unitSize = 1, bool airPath = false);

	// ToDo: What about air? Only air units might block a path
	Path findPath(); // ToDo: Maybe parameter "bool closest" such that if a tile cannot be reached the next close point is taken

	inline const std::unordered_set<INode*>& debug_openList() const {
		return m_setOpenList;
	}

	inline const std::unordered_set<INode*>& debug_closedList() const {
		return m_setClosedList;
	}

private:
	std::vector<INode*> buildFinalPath(INode* pNode);
	INode* getNodeWith_fMin();
	int heuristic(const INode* pNode);

	INode* m_pEndNode;
	int m_unitSize;
	bool m_airPath;

	// Maps f values [key] to appropriate INodes. 
	// Important that an ordered map is used as the lowest f value is required in 'getNodeWith_fMin', obtained by 'm_mapOpenList_f_values.begin()'
	std::map<int, std::unordered_set<INode*>> m_mapOpenList_f_values;
	std::unordered_set<INode*> m_setOpenList;
	std::unordered_set<INode*> m_setClosedList;
};
