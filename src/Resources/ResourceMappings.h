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
#include <typeindex>
#include <string>

// Forward declarations
class Peasant;
class Archer;
class Ballista;
class Footman;
class Knight;
class Mage;
class Dwarfes;
class Skeleton;
class Battleship;
class Destroyer;
class GnomishSubmarine;
class Tanker;
class Transport;
class GnomishFlyingMaschine;
class GryphonRider;

class Barracks;
class Blacksmith;
class Church;
class ScoutTower;
class Townhall;
class GnomishInventor;
class GryphonAviary;
class OilRig;
class OilRefinery;
class Shipyard;
class Stables;
class Farm;
class Foundry;
class LumberMill;
class MageTower;
class DarkPortal;
class GoldMine;

using MapFileNameToTypeID = std::unordered_map<std::string, std::type_index>;

namespace ResourceMappings {
    const MapFileNameToTypeID& getHumanBuildingMap();
    const MapFileNameToTypeID& getOrcBuildingMap();
    const MapFileNameToTypeID& getNeutralBuildingMap();
    const MapFileNameToTypeID& getUnitTypeMap();
}