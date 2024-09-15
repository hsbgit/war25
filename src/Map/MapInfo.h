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

#include "Map.h"
#include <vector>


// Owned by each player
class MapInfo {
public:
    MapInfo(const Map& map);

    void updateMapVisibility(const std::vector<Object*>& vecObjects);

    double exploredAreaInPercent() const {
        return (double)m_mapExplored.size() / double(m_map.sideLength() * m_map.sideLength()) * 100.0;
    }

    std::vector<std::vector<bool> > m_mapExplored; // Not explored: Black on screen / minimap
    std::vector<std::vector<bool> > m_mapCurrentlyVisible; // Explored but not visible by any units: fog of war!

private:
    const Map& m_map;
};
