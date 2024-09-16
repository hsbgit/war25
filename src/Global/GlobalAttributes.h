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

#include <typeinfo>
#include <typeindex>
#include <unordered_map>

#include "Resources.h"


// Global var used inside the building constructor to calculate how many ticks are required for each health point when constructing the building
// tick_ms can be changed by the programmer (see main.cpp) that's why the entities should not assume a specific value.
extern int g_tick_ms;

int calculateRequiredTicksForGameAction(int requiredTime_ms);

/*
 * In specific situations we need those attributes BEFORE creating a unit.
 * E.g., considerung calculating whether the player has enough resources to produce a building.
 * Or if a unit needs to be placed the tilesize has to be known beforehand.
 *
 * Access the desired information like this:
 *
 *      gMapUnitTilesize[typeid(Barracks)]
 *
 * or
 *
 *      gMapProductionTimes_ms[typeid(*pObject)]
 */
extern std::unordered_map<std::type_index, int> gMap_Neutral_Units_2_PudTextureIds;
extern std::unordered_map<std::type_index, int> gMap_Alliance_Units_2_PudTextureIds;

extern std::unordered_map<std::type_index, ProductionCosts> gMapProductionCosts;
extern std::unordered_map<std::type_index, int> gMapProductionTimes_ms;
extern std::unordered_map<std::type_index, int> gMapUnitTilesize; // 1x1, 2x2, 3x3, max. 4x4

enum ObjectType { Land, Sea, Air, Coastal }; // Coastal: Harbour building etc. which use coastal tiles as well as water tiles
extern std::unordered_map<std::type_index, ObjectType> gMapUnitType;
