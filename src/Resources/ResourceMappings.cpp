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
#include "ResourceMappings.h"

#include "Units/LandUnits/Peasant/Peasant.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Footman.h"
#include "Units/LandUnits/Knight.h"
#include "Units/LandUnits/Mage.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"
#include "Units/LandUnits/Skeleton.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"

#include "Buildings/Barracks.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Church.h"
#include "Buildings/ScoutTower.h"
#include "Buildings/Townhall.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/OilRig.h"
#include "Buildings/OilRefinery.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Stables.h"
#include "Buildings/Farm.h"
#include "Buildings/Foundry.h"
#include "Buildings/LumberMill.h"
#include "Buildings/MageTower.h"
#include "Buildings/DarkPortal.h"
#include "Buildings/Goldmine.h"

namespace ResourceMappings {

const MapFileNameToTypeID& getHumanBuildingMap() {
    static const MapFileNameToTypeID humanBuildings{
        {"barracks.png", typeid(Barracks)},
        {"blacksmith.png", typeid(Blacksmith)},
        {"cannon_tower.png", typeid(ScoutTower)},
        {"castle.png", typeid(Townhall)},
        {"church.png", typeid(Church)},
        {"elven_lumber_mill.png", typeid(LumberMill)},
        {"farm.png", typeid(Farm)},
        {"foundry.png", typeid(Foundry)},
        {"gnomish_inventor.png", typeid(GnomishInventor)},
        {"gryphon_aviary.png", typeid(GryphonAviary)},
        {"guard_tower.png", typeid(ScoutTower)},
        {"keep.png", typeid(Townhall)},
        {"mage_tower.png", typeid(MageTower)},
        {"oil_platform.png", typeid(OilRig)},
        {"refinery.png", typeid(OilRefinery)},
        {"scout_tower.png", typeid(ScoutTower)},
        {"shipyard.png", typeid(Shipyard)},
        {"stables.png", typeid(Stables)},
        {"town_hall.png", typeid(Townhall)}
    };
    return humanBuildings;
}

const MapFileNameToTypeID& getOrcBuildingMap() {
    static const MapFileNameToTypeID orcBuildings{
        {"barracks.png", typeid(Barracks)},
        {"blacksmith.png", typeid(Blacksmith)},
        {"watch_tower.png", typeid(ScoutTower)},
        {"cannon_tower.png", typeid(ScoutTower)},
        {"guard_tower.png", typeid(ScoutTower)},
        {"fortress.png", typeid(Townhall)},
        {"stronghold.png", typeid(Townhall)},
        {"great_hall.png", typeid(Townhall)},
        {"altar_of_storms.png", typeid(Church)},
        {"troll_lumber_mill.png", typeid(LumberMill)},
        {"pig_farm.png", typeid(Farm)},
        {"foundry.png", typeid(Foundry)},
        {"oil_platform.png", typeid(OilRig)},
        {"refinery.png", typeid(OilRefinery)},
        {"shipyard.png", typeid(Shipyard)},
        {"ogre_mound.png", typeid(Stables)},
        {"goblin_alchemist.png", typeid(GnomishInventor)},
        {"dragon_roost.png", typeid(GryphonAviary)},
        {"temple_of_the_damned.png", typeid(MageTower)}
    };
    return orcBuildings;
}

const MapFileNameToTypeID& getNeutralBuildingMap() {
    static const MapFileNameToTypeID neutralBuildings{
        {"dark_portal.png", typeid(DarkPortal)},
        {"gold_mine.png", typeid(GoldMine)}
    };
    return neutralBuildings;
}

const MapFileNameToTypeID& getUnitTypeMap() {
    static const MapFileNameToTypeID unitTypes{
        // Neutral
        {"corpses.png", typeid(Peasant)},
        {"daemon.png", typeid(Ballista)},
        {"skeleton.png", typeid(Skeleton)},

        // Human
        {"ballista.png", typeid(Ballista)},
        {"battleship.png", typeid(Battleship)},
        {"dwarven_demolition_squad.png", typeid(Dwarfes)},
        {"elven_archer.png", typeid(Archer)},
        {"elven_destroyer.png", typeid(Destroyer)},
        {"footman.png", typeid(Footman)},
        {"gnomish_flying_machine.png", typeid(GnomishFlyingMaschine)},
        {"gnomish_submarine.png", typeid(GnomishSubmarine)},
        {"gryphon_rider.png", typeid(GryphonRider)},
        {"knight.png", typeid(Knight)},
        {"mage.png", typeid(Mage)},
        {"oil_tanker_empty.png", typeid(Tanker)},
        {"oil_tanker_full.png", typeid(Tanker)},
        {"peasant.png", typeid(Peasant)},
        {"peasant_with_gold.png", typeid(Peasant)},
        {"peasant_with_wood.png", typeid(Peasant)},
        {"transport.png", typeid(Transport)},

        // Orcs
        {"catapult.png", typeid(Ballista)},
        {"ogre_juggernaught.png", typeid(Battleship)},
        {"goblin_sappers.png", typeid(Dwarfes)},
        {"troll_axethrower.png", typeid(Archer)},
        {"troll_destroyer.png", typeid(Destroyer)},
        {"grunt.png", typeid(Footman)},
        {"goblin_zeppelin.png", typeid(GnomishFlyingMaschine)},
        {"giant_turtle.png", typeid(GnomishSubmarine)},
        {"dragon.png", typeid(GryphonRider)},
        {"ogre.png", typeid(Knight)},
        {"death_knight.png", typeid(Mage)},
        {"peon.png", typeid(Peasant)},
        {"peon_with_gold.png", typeid(Peasant)},
        {"peon_with_wood.png", typeid(Peasant)},
        {"eye_of_kilrogg.png", typeid(Ballista)}
    };
    return unitTypes;
}

} // namespace ResourceMappings