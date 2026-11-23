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
#include "Sprite.h"
#include "EventHandling/EventHandlerID.h"
#include <algorithm>
#include <map>
#include <vector>
#include <unordered_set>


// Owned by each player
class MapInfo {
public:
	struct TimedReveal {
		Point center{0, 0};
		int radius = 0;
		int remainingTicks = 0;
	};
	struct RememberedBuilding {
		EventProcessorID id = 0;
		Point topLeft{0, 0};
		int tileSize = 1;
		Sprite sprite;
	};

    MapInfo(const Map& map);

    void updateMapVisibility(const std::vector<Object*>& vecObjects);
	void revealArea(const Point& center, int radius, int durationMs);

    double exploredAreaInPercent() const {
		std::size_t explored = 0;
		for (const auto& row : m_mapExplored)
			explored += static_cast<std::size_t>(std::count(row.begin(), row.end(), true));
		return static_cast<double>(explored) /
			static_cast<double>(m_map.sideLength() * m_map.sideLength()) * 100.0;
    }

    std::vector<std::vector<bool> > m_mapExplored; // Not explored: Black on screen / minimap
    std::vector<std::vector<bool> > m_mapCurrentlyVisible; // Explored but not visible by any units: fog of war!

	const std::map<EventProcessorID, RememberedBuilding>& rememberedBuildings() const {
		return m_rememberedBuildings;
	}
	bool isAreaCurrentlyVisible(const Point& topLeft, int tileSize) const;
	bool canCurrentlySee(const Object* object) const;

private:
	void updateRememberedBuildings();
	const Map& m_map;
	std::map<EventProcessorID, RememberedBuilding> m_rememberedBuildings;
	std::unordered_set<const Object*> m_detectedSubmergedUnits;
	std::vector<TimedReveal> m_timedReveals;
};
