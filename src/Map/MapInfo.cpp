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
#include "MapInfo.h"


MapInfo::MapInfo(const Map& map) : m_map(map) {
    std::size_t dimensions = m_map.sideLength();

    // https://stackoverflow.com/questions/22983707/stdvectorbool-guaranteed-to-default-all-entries-to-false
    m_mapExplored.resize(dimensions);
    m_mapCurrentlyVisible.resize(dimensions);

    for (std::size_t y = 0; y < dimensions; ++y) {
        m_mapExplored[y].resize(dimensions);
        m_mapCurrentlyVisible[y].resize(dimensions);
    }

}


void MapInfo::updateMapVisibility(const std::vector<Object*>& vecObjects) {
    for (auto& pObj : vecObjects) {
        if (!pObj->isPlacedOnMap())
            continue;

        std::vector<Tile*> vecVisibleTiles;
        m_map.getVisibleTiles(pObj, pObj->getSight(), vecVisibleTiles);

        for (auto& pTile : vecVisibleTiles) {
            m_mapExplored[pTile->getY()][pTile->getX()] = true;
            m_mapCurrentlyVisible[pTile->getY()][pTile->getX()] = true;
        }
    }
}
