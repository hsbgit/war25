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

#include <cassert>
#include <vector>

class INode;

class Path {
public:
	std::vector<INode*> m_vecPathNodes; // Could be a set as a path should contain each tile just once. But as a set is not ordered sequentially the path would get destroyed.

	INode* getFront() const {
		assert(isValid());
		return m_vecPathNodes.front();
	}

	void popFront() {
		assert(isValid());
		m_vecPathNodes.erase(m_vecPathNodes.begin());
	}

	inline bool isValid() const {
		return m_vecPathNodes.size();
	}

	void clear() {
		m_vecPathNodes.clear();
	}

	bool m_isValid = false;
};
