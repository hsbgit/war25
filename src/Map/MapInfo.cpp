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
#include "Buildings/Building.h"
#include "Player.h"
#include "Global/GlobalAttributes.h"

#include <algorithm>
#include <set>


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
	for (auto& row : m_mapCurrentlyVisible)
		std::fill(row.begin(), row.end(), false);

    for (auto& pObj : vecObjects) {
        if (!pObj->isPlacedOnMap())
            continue;

        std::vector<Tile*> vecVisibleTiles;
        m_map.getVisibleTiles(pObj, pObj->getEffectiveSight(), vecVisibleTiles);

        for (auto& pTile : vecVisibleTiles) {
            m_mapExplored[pTile->getY()][pTile->getX()] = true;
            m_mapCurrentlyVisible[pTile->getY()][pTile->getX()] = true;
        }
    }

	updateRememberedBuildings();
}

bool MapInfo::isAreaCurrentlyVisible(const Point& topLeft, int tileSize) const {
	for (int y = topLeft.y; y < topLeft.y + tileSize; ++y)
		for (int x = topLeft.x; x < topLeft.x + tileSize; ++x)
			if (y >= 0 && x >= 0 && y < static_cast<int>(m_mapCurrentlyVisible.size()) &&
				x < static_cast<int>(m_mapCurrentlyVisible[y].size()) && m_mapCurrentlyVisible[y][x])
				return true;
	return false;
}

void MapInfo::updateRememberedBuildings() {
	std::set<EventProcessorID> existingBuildings;
	for (const auto& [color, player] : m_map.m_mapPlayer) {
		for (Object* object : player->getObjects()) {
			auto* building = dynamic_cast<Building*>(object);
			if (!building || !building->isPlacedOnMap() || building->isDead()) continue;
			const EventProcessorID id = building->getID();
			existingBuildings.insert(id);
			const Point topLeft = building->getTileLocation();
			const int tileSize = gMapUnitTilesize[typeid(*building)];
			if (isAreaCurrentlyVisible(topLeft, tileSize))
				m_rememberedBuildings[id] = RememberedBuilding{id, topLeft, tileSize, building->sprite()};
		}
	}

	std::erase_if(m_rememberedBuildings, [&](const auto& entry) {
		const RememberedBuilding& remembered = entry.second;
		return !existingBuildings.contains(entry.first) &&
			isAreaCurrentlyVisible(remembered.topLeft, remembered.tileSize);
	});
}
