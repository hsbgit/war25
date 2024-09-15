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

#include <unordered_map>
#include "Direction.h"

class INode;
typedef std::unordered_map<Direction, INode*> NeighbourNodes;

class INode {
public:
    virtual ~INode(){ }

    /*
     * Must return all neighbournodes that are actually usable when finding a path.
     * The term 'Usable' means that a unit can walk on it (water units only on water etc.)
     * and there are not other units blocking the path.
     *
     * Air path may only be restricted by user units
     */
    virtual NeighbourNodes getFreeNeighbourNodes(int unitSize_tiles = 1, bool airPath = false) const = 0;

    virtual int getX() const = 0;
    virtual int getY() const = 0;

    INode* m_pParentNode = nullptr;

    inline int getF() const {
        return m_g + m_h;
    }

    int m_g = 0;
    int m_h = 0;
};
