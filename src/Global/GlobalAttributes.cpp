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

#include "GlobalAttributes.h"
#include "Buildings/Barracks.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Church.h"
#include "Buildings/Farm.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/Foundry.h"
#include "Buildings/Goldmine.h"
#include "Buildings/LumberMill.h"
#include "Buildings/MageTower.h"
#include "Buildings/ScoutTower.h"
#include "Buildings/OilRefinery.h"
#include "Buildings/OilRig.h"


#include "Buildings/DarkPortal.h"
#include "Buildings/ScoutTower.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Stables.h"
#include "Buildings/Townhall.h"
#include "Units/Critter.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"


#include "Units/LandUnits/Peasant/Peasant.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Footman.h"
#include "Units/LandUnits/Knight.h"
#include "Units/LandUnits/Mage.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"





int g_tick_ms;


int calculateRequiredTicksForGameAction(int requiredTime_ms) {
	assert(requiredTime_ms % g_tick_ms == 0); // Must be perfectly synchronized to dt_ms
	int requiredTicks = requiredTime_ms / g_tick_ms;
	return requiredTicks;
}




// Goldmine, Monolith etc.
std::unordered_map<std::type_index, std::unordered_map<Pud_Era, int> > gMap_Neutral_BuildingEra_2_PudTextureIds{
{typeid(GoldMine), { {PUD_ERA_FOREST, 119}} },
{typeid(GoldMine), { {PUD_ERA_WINTER, 162}} },
{typeid(GoldMine), { {PUD_ERA_WASTELAND, 179}} },
{typeid(GoldMine), { {PUD_ERA_SWAMP, 511}} }
/*
{typeid(PUD_UNIT_OIL_PATCH), { {PUD_ERA_FOREST, 118}} },
{typeid(PUD_UNIT_OIL_PATCH), { {PUD_ERA_WINTER, 118}} },
{typeid(PUD_UNIT_OIL_PATCH), { {PUD_ERA_WASTELAND, 180}} },
{typeid(PUD_UNIT_OIL_PATCH), { {PUD_ERA_SWAMP, 515}} }


   case PUD_UNIT_DARK_PORTAL         : NEUTRAL_BUILDING_SWITCH(167, 184, 185, 513); break;
   case PUD_UNIT_RUNESTONE           : NEUTRAL_BUILDING_SWITCH(181, 186, 181, 514); break;
   case PUD_UNIT_CIRCLE_OF_POWER     : NEUTRAL_BUILDING_SWITCH(166, 166, 166, 525);
	*/
};


std::unordered_map<std::type_index, std::unordered_map<Pud_Era, int> > gMap_Alliance_BuildingEra_2_PudTextureIds{
	// Alliance
	{typeid(Townhall), { {PUD_ERA_FOREST, 100}} },
	{typeid(Townhall), { {PUD_ERA_WINTER, 142}} },
	{typeid(Townhall), { {PUD_ERA_WASTELAND, 100}} },
	{typeid(Townhall), { {PUD_ERA_SWAMP, 487}} },

	{typeid(Barracks), { {PUD_ERA_FOREST, 94}} },
	{typeid(Barracks), { {PUD_ERA_WINTER, 136}} },
	{typeid(Barracks), { {PUD_ERA_WASTELAND, 94}} },
	{typeid(Barracks),{ {PUD_ERA_SWAMP, 481} }},

	{typeid(Blacksmith), { {PUD_ERA_FOREST, 106}} },
	{typeid(Blacksmith), { {PUD_ERA_WINTER, 148}} },
	{typeid(Blacksmith), { {PUD_ERA_WASTELAND, 106}} },
	{typeid(Blacksmith),{ {PUD_ERA_SWAMP, 493} }},

	{typeid(Farm), { {PUD_ERA_FOREST, 92}} },
	{typeid(Farm), { {PUD_ERA_WINTER, 134}} },
	{typeid(Farm), { {PUD_ERA_WASTELAND, 173}} },
	{typeid(Farm), { {PUD_ERA_SWAMP, 479}} },

	{typeid(LumberMill), { {PUD_ERA_FOREST, 102}} },
	{typeid(LumberMill), { {PUD_ERA_WINTER, 144}} },
	{typeid(LumberMill), { {PUD_ERA_WASTELAND, 175}} },
	{typeid(LumberMill), { {PUD_ERA_SWAMP, 489}} },

	{typeid(ScoutTower), { {PUD_ERA_FOREST, 98}} },
	{typeid(ScoutTower), { {PUD_ERA_WINTER, 140}} },
	{typeid(ScoutTower), { {PUD_ERA_WASTELAND, 98}} },
	{typeid(ScoutTower), { {PUD_ERA_SWAMP, 485}} },

	{typeid(Shipyard), { {PUD_ERA_FOREST, 108}} },
	{typeid(Shipyard), { {PUD_ERA_WINTER, 150}} },
	{typeid(Shipyard), { {PUD_ERA_WASTELAND, 108}} },
	{typeid(Shipyard), { {PUD_ERA_SWAMP, 495}} },

	{typeid(OilRig), { {PUD_ERA_FOREST, 114}} },
	{typeid(OilRig), { {PUD_ERA_WINTER, 156}} },
	{typeid(OilRig), { {PUD_ERA_WASTELAND, 177}} },
	{typeid(OilRig), { {PUD_ERA_SWAMP, 501}} },

	{typeid(Foundry), { {PUD_ERA_FOREST, 110}} },
	{typeid(Foundry), { {PUD_ERA_WINTER, 152}} },
	{typeid(Foundry), { {PUD_ERA_WASTELAND, 110}} },
	{typeid(Foundry), { {PUD_ERA_SWAMP, 497}} },

	{typeid(OilRefinery), { {PUD_ERA_FOREST, 112}} },
	{typeid(OilRefinery), { {PUD_ERA_WINTER, 154}} },
	{typeid(OilRefinery), { {PUD_ERA_WASTELAND, 112}} },
	{typeid(OilRefinery), { {PUD_ERA_SWAMP, 499}} },

	{typeid(Stables), { {PUD_ERA_FOREST, 104}} },
	{typeid(Stables), { {PUD_ERA_WINTER, 146}} },
	{typeid(Stables), { {PUD_ERA_WASTELAND, 173}} },
	{typeid(Stables), { {PUD_ERA_SWAMP, 491}} },


	{typeid(GnomishInventor), { {PUD_ERA_FOREST, 90}} },
	{typeid(GnomishInventor), { {PUD_ERA_WINTER, 132}} },
	{typeid(GnomishInventor), { {PUD_ERA_WASTELAND, 90}} },
	{typeid(GnomishInventor), { {PUD_ERA_SWAMP, 477}} },

	{typeid(GryphonAviary), { {PUD_ERA_FOREST, 88}} },
	{typeid(GryphonAviary), { {PUD_ERA_WINTER, 130}} },
	{typeid(GryphonAviary), { {PUD_ERA_WASTELAND, 88}} },
	{typeid(GryphonAviary), { {PUD_ERA_SWAMP, 475}} },

	{typeid(MageTower), { {PUD_ERA_FOREST, 84}} },
	{typeid(MageTower), { {PUD_ERA_WINTER, 160}} },
	{typeid(MageTower), { {PUD_ERA_WASTELAND, 84}} },
	{typeid(MageTower), { {PUD_ERA_SWAMP, 505}} },


	{typeid(Church), { {PUD_ERA_FOREST, 96}} },
	{typeid(Church), { {PUD_ERA_WINTER, 138}} },
	{typeid(Church), { {PUD_ERA_WASTELAND, 96}} },
	{typeid(Church), { {PUD_ERA_SWAMP, 483}} }
};






std::unordered_map<std::type_index, int> gMap_Neutral_Units_2_PudTextureIds{
	{typeid(Critter), 64}
};


std::unordered_map<std::type_index, int> gMap_Alliance_Units_2_PudTextureIds{
	{typeid(Peasant), 47},
	{typeid(Footman), 45},
	{typeid(Archer), 53},
	{typeid(Knight), 51},
	{typeid(Ballista), 49},
	{typeid(Mage), 55},
	{typeid(Dwarfes), 33},
	{typeid(GnomishFlyingMaschine), 38},
	{typeid(GryphonRider), 35},
	{typeid(Tanker), 59},
	{typeid(Transport), 39},
	{typeid(Destroyer), 61},
	{typeid(Battleship), 41},
	{typeid(GnomishSubmarine), 43}
};


std::unordered_map<std::type_index, ProductionCosts> gMapProductionCosts = {
	// Units
	{typeid(Peasant), ProductionCosts(400)},
	{typeid(Footman), ProductionCosts(600)},
	{typeid(Archer), ProductionCosts(500,50)},
	{typeid(Knight), ProductionCosts(800,100)},
	{typeid(Ballista), ProductionCosts(900,300)},

	// Buildings
	{typeid(Barracks), ProductionCosts(700,400)},
	{typeid(Blacksmith), ProductionCosts(800,450)},
	{typeid(Farm), ProductionCosts(500,250)},
	{typeid(GnomishInventor), ProductionCosts(9999,9999,9999)},
	{typeid(GryphonAviary), ProductionCosts(9999,9999,9999)},
	{typeid(Foundry), ProductionCosts(9999,9999,9999)},
	{typeid(LumberMill), ProductionCosts(600,450)},
	{typeid(MageTower), ProductionCosts(9999,9999,9999)},
	{typeid(OilRefinery), ProductionCosts(9999,9999,9999)},
	{typeid(ScoutTower), ProductionCosts(550,200)}, // Upgrade costs are handled internally in class 'ScoutTower'
	{typeid(Townhall), ProductionCosts(1200,800)} // Upgrade costs are handled internally in class 'Townhall'
};


std::unordered_map<std::type_index, int> gMapProductionTimes_ms = {
	// Units
	{typeid(Peasant), 45 * 1000},
	{typeid(Footman), 1 * 1000}, // 60
	{typeid(Archer), 1 * 1000},  // 70
	{typeid(Knight), 1 * 1000}, // 90
	{typeid(Ballista), 250 * 1000},

	// Buildings
	{typeid(Barracks), 200 * 1000},
	{typeid(Blacksmith), 200 * 1000},
	{typeid(Farm), 100 * 1000},

	{typeid(LumberMill), 150 * 1000},
	{typeid(ScoutTower), 5 * 1000}, // 60 Upgrade times are( identical?? )handled internally in class 'ScoutTower'
	{typeid(Townhall), 200 * 1000}, // Upgrade times are( identical?? )handled internally in class 'Townhall'


	// Todo korrekte werte


	{typeid(GnomishInventor), 987 * 1000},
	{typeid(GryphonAviary), 987 * 1000},
	{typeid(Foundry), 987 * 1000},
	{typeid(MageTower), 987 * 1000},
	{typeid(OilRefinery), 987 * 1000},


	{typeid(Stables), 100 * 1000}
};


std::unordered_map<std::type_index, int> gMapUnitTilesize = {
	// Units
	{typeid(Critter), 1},
	{typeid(Peasant), 1},
	{typeid(Footman), 1},
	{typeid(Archer), 1},
	{typeid(Knight), 1},
	{typeid(Ballista), 1},
	{typeid(Mage), 1},
	{typeid(Dwarfes), 1},
	{typeid(GnomishFlyingMaschine), 2},
	{typeid(GryphonRider), 2},
	{typeid(Tanker), 2},
	{typeid(Transport), 2},
	{typeid(Destroyer), 2},
	{typeid(Battleship), 2},
	{typeid(GnomishSubmarine), 2},

	// Buildings
	{typeid(GoldMine), 3},
	{typeid(DarkPortal), 4},

	{typeid(Barracks), 3},
	{typeid(Blacksmith), 3},
	{typeid(Church), 3},
	{typeid(Farm), 2},
	{typeid(Foundry), 3},
	{typeid(LumberMill), 3},
	{typeid(OilRefinery), 3},



	{typeid(GnomishInventor), 3},
	{typeid(GryphonAviary), 3},
	{typeid(MageTower), 3},
	{typeid(ScoutTower), 2},
	{typeid(Shipyard), 3},
	{typeid(Stables), 3},
	{typeid(Townhall), 4},

	{typeid(OilRig), 3}
};


std::unordered_map<std::type_index, ObjectType> gMapUnitType = {
	// Units
	{typeid(Critter), ObjectType::Land},
	{typeid(Peasant), ObjectType::Land},
	{typeid(Footman), ObjectType::Land},
	{typeid(Archer), ObjectType::Land},
	{typeid(Knight), ObjectType::Land},
	{typeid(Ballista), ObjectType::Land},

	// Buildings
	{typeid(GoldMine), ObjectType::Land},

	{typeid(Barracks), ObjectType::Land},
	{typeid(Church), ObjectType::Land},
	{typeid(Farm), ObjectType::Land},
	{typeid(GnomishInventor), ObjectType::Land},
	{typeid(GryphonAviary), ObjectType::Land},
	{typeid(MageTower), ObjectType::Land},
	{typeid(OilRig), ObjectType::Sea},
	{typeid(OilRefinery), ObjectType::Sea}, // type: shore??


	{typeid(ScoutTower), ObjectType::Land},
	{typeid(Shipyard), ObjectType::Land},
	{typeid(Townhall), ObjectType::Land}
};

