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


// ----------------------------------------------------------------------------
// !!! ToDo: This file needs a big refactoring !!!
// ----------------------------------------------------------------------------


#include "resourcemanager_wartools.h"
#include "Texture.h"
#include "Sprite.h"
#include "Renderer.h"
#include "PlayerColor.h"

#include "Buildings/DarkPortal.h"
#include "Buildings/MageTower.h"
#include "Buildings/LumberMill.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#include "Units/LandUnits/Peasant/Peasant.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Footman.h"
#include "Units/LandUnits/Knight.h"
#include "Units/LandUnits/Mage.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"
#include "Units/LandUnits/Skeleton.h"



void ResourceManager_wartools::parseLUATileSetFile(Season season, const std::string& strFile) {
	std::ifstream file(strFile);

	if (!file) {
		throw std::logic_error("Unable to open LUA file!");
	}


	int tile_id = 0x000;
	std::string s;
	while (std::getline(file, s))
	{
		// Second condition avoids rows which have "--" but do not represent tile id values (look at file...)
		if ((s.find("--") != std::string::npos) && (s.substr(0, 4) == "    ")) {
			s.erase(std::remove(s.begin(), s.end(), '{'), s.end());
			s.erase(std::remove(s.begin(), s.end(), '}'), s.end());
			s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

			// Special case: Last relevant address entry;
			// Has no comma at required position -> we would cut a valid value!
			// So we cut only if it is not the 9D0 row
			if (s.find("9D0") == std::string::npos) {
				// Cut everything from last ',' till end of line
				auto itLast = s.find_last_of(',');
				s = s.erase(itLast, s.size());
			}

			//std::cerr<<strFile<<"\t"<<std::hex<<tile_id<<"\t"<<std::dec<<s.size()<<"\t"<<s<<std::endl;

			if (s.size()) {
				std::string entry;
				std::stringstream ss(s);

				int sub_tile_id = tile_id;
				while (getline(ss, entry, ',')) {
					try {
						int index = std::stoi(entry);

						if (index != 0) {
							if (index == 142) {
								std::cerr << std::hex << sub_tile_id << std::endl;
							}
							m_mapPUDId_to_TileSetIndex[sub_tile_id][season] = index;
							// std::cerr<<"\t\t"<<std::hex<<sub_tile_id<<"\t"<<std::dec<<index<<std::endl;
						}
					}
					catch (...) {

					}

					++sub_tile_id;
				}
			}

			tile_id += 0x010;
		}
	}

	m_mapPUDId_to_TileSetIndex[SpecialTile::RemovedWood][season] = 126; // How to get this: Look at *.png and cnt rows and columns: 7*16+14 = 126
	m_mapPUDId_to_TileSetIndex[SpecialTile::RemovedRock][season] = 10 * 16 + 6;

	// Todo: implement all walls correct
	m_mapPUDId_to_TileSetIndex[SpecialTile::DestroyedWalls][season] = 5 * 16 + 8;


	int indexDestroyedWallsBegin = 5 * 16 + 8;
	int cntTiles = 14;

	for (int i = indexDestroyedWallsBegin; i < indexDestroyedWallsBegin + cntTiles; ++i) {
		//  m_mapPUDId_to_TileSetIndex[i][season] = 
	}


}


ResourceManager_wartools* g_pRessourceManager = nullptr;


const std::string ResourceManager_wartools::strPrefix = std::string("data") + PATH_SEPARATOR + std::string("graphics") + PATH_SEPARATOR;


// Unfortunately required as SDL_SetCursor does not always work properly -> Windows default cursor is set again so we call refreshLoadedCursor
void ResourceManager_wartools::refreshLoadedCursor() {
	if (m_pLastSetCursor) {
		SDL_SetCursor(m_pLastSetCursor);
	}
}


void ResourceManager_wartools::setCursor(CursorTypes type) {
	assert(m_cursors.contains(type));
	SDL_SetCursor(m_cursors[type]);
	m_pLastSetCursor = m_cursors[type];
}


void ResourceManager_wartools::loadCursors() {
	// Human Cursors
	std::string strHumanPath = strPrefix + "ui" + PATH_SEPARATOR + "human" + PATH_SEPARATOR + "cursors" + PATH_SEPARATOR;

	SDL_Surface* pSurface = IMG_Load((strHumanPath + "red_eagle.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::RedEagle] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strHumanPath + "yellow_eagle.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::YellowEagle] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strHumanPath + "green_eagle.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::GreenEagle] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strHumanPath + "human_gauntlet.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::HumanGauntlet] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);



	// Orc Cursors
	std::string strOrcPath = strPrefix + "ui" + PATH_SEPARATOR + "orc" + PATH_SEPARATOR + "cursors" + PATH_SEPARATOR;

	pSurface = IMG_Load((strOrcPath + "red_crosshairs.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::RedCrosshairs] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strOrcPath + "yellow_crosshairs.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::YellowCrosshairs] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strOrcPath + "green_crosshairs.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::GreenCrosshairs] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strOrcPath + "orcish_claw.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::OrcishClaw] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	// Neutral
	pSurface = IMG_Load((strPrefix + "ui" + PATH_SEPARATOR + "cursors" + PATH_SEPARATOR + "magnifying_glass.png").c_str());
	assert(pSurface);
	m_cursors[CursorTypes::MagnifyingGlass] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);
}




#include "PlayerColor.h"
#include <map>


enum class eBuilding { Farm, Barracks };

class BuildingContainer {
public:
	std::map<eBuilding, Sprite*> vecBuildings;
	std::map<eBuilding, Sprite*> vecBuildingsUnderConstruction;
	std::map<eBuilding, Sprite*> vecBuildingsInUse;
};

class AllianceContainer {
public:
	std::map<PlayerColor, BuildingContainer> mapBuildings;
};

enum class eTileSet { Summer, Winter, Wasteland };



#include "Animation.h"
#include "Units/Unit.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"

#include <unordered_map>


typedef std::unordered_map<std::string, std::type_index> MapFileNameToTypeID;


// ToDo: Common header to include for simplicity?
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

// Human
MapFileNameToTypeID mapPNGFileToBuilding{
	{"barracks.png", typeid(Barracks)},
	{"blacksmith.png", typeid(Blacksmith)},
	{"cannon_tower.png", typeid(ScoutTower)}, // Special case
	{"castle.png", typeid(Townhall)}, // Special case   
	{"church.png", typeid(Church)},
	{"elven_lumber_mill.png", typeid(LumberMill)},
	{"farm.png", typeid(Farm)},
	{"foundry.png", typeid(Foundry)},
	{"gnomish_inventor.png", typeid(GnomishInventor)},
	{"gryphon_aviary.png", typeid(GryphonAviary)},
	{"guard_tower.png", typeid(ScoutTower)},// Special case
	{"keep.png", typeid(Townhall)}, // Special case
	{"mage_tower.png", typeid(MageTower)},
	{"oil_platform.png", typeid(OilRig)},
	{"refinery.png", typeid(OilRefinery)},
	{"scout_tower.png", typeid(ScoutTower)},// Special case
	{"shipyard.png", typeid(Shipyard)},
	{"stables.png", typeid(Stables)},
	{"town_hall.png", typeid(Townhall)} // Special case


	// oil_well_construction_site.png bzw. bei swamp oil_platform_construction_size
	// refinery_construction_site
	// shipyard_construction_site
};


// Orc
MapFileNameToTypeID mapPNGFileToBuilding_Orc{
	{"barracks.png", typeid(Barracks)},
	{"blacksmith.png", typeid(Blacksmith)},
	{"watch_tower.png", typeid(ScoutTower)},// Special case
	{"cannon_tower.png", typeid(ScoutTower)}, // Special case
	{"guard_tower.png", typeid(ScoutTower)},// Special case
	{"fortress.png", typeid(Townhall)}, // Special case
	{"stronghold.png", typeid(Townhall)}, // Special case
	{"great_hall.png", typeid(Townhall)}, // Special case
	{"altar_of_storms.png", typeid(Church)},
	{"troll_lumber_mill.png", typeid(LumberMill)},
	{"pig_farm.png", typeid(Farm)},
	{"foundry.png", typeid(Foundry)},
	{"oil_platform.png", typeid(OilRig)},
	// oil_well_construction_site.png
	{"refinery.png", typeid(OilRefinery)},
	{"shipyard.png", typeid(Shipyard)},
	{"ogre_mound.png", typeid(Stables)},
	{"goblin_alchemist.png", typeid(GnomishInventor)},
	{"dragon_roost.png", typeid(GryphonAviary)},
	{"temple_of_the_damned.png", typeid(MageTower)}
};




#include "Buildings/Goldmine.h"
#include "Buildings/Building.h"
MapFileNameToTypeID mapPNGFileToBuilding_Neutral{
	{"dark_portal.png", typeid(DarkPortal)},
	// {"destroyed_site.png", typeid(GoldMine)}, // fix me
	 {"gold_mine.png", typeid(GoldMine)}
	 /*
	 {"oil_patch.png", typeid(Barracks)},
	 {"small_destroyed_site.png", typeid(Barracks)},
	 {"wall.png", typeid(Barracks)},
	 {"wall_construction_site.png", typeid(Blacksmith)}*/
};



// neutral
MapFileNameToTypeID mapPNGFileToType = {
	// Neutral 
	{"corpses.png", typeid(Peasant)},
	{"daemon.png", typeid(Ballista)}, // Dummy
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
	{"oil_tanker_empty.png", typeid(Tanker)},         // Problem!!!!
	{"oil_tanker_full.png", typeid(Tanker)},           // Problem!!!!
	{"peasant.png", typeid(Peasant)},                 // Problem!!!!
	{"peasant_with_gold.png", typeid(Peasant)},       // Problem!!!!
	{"peasant_with_wood.png", typeid(Peasant)},       // Problem!!!!
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
	{"oil_tanker_empty.png", typeid(Tanker)},         // Problem!!!!
	{"oil_tanker_full.png", typeid(Tanker)},           // Problem!!!!
	{"peon.png", typeid(Peasant)},                 // Problem!!!!
	{"peon_with_gold.png", typeid(Peasant)},       // Problem!!!!
	{"peon_with_wood.png", typeid(Peasant)},       // Problem!!!!
	{"transport.png", typeid(Transport)},
	{"eye_of_kilrogg.png", typeid(Ballista)} }; // Problem!!!! (maps currently to a dummy type)


#include "Player.h"
#include "Global/GlobalAttributes.h"
#include "Units/Critter.h"


void ResourceManager_wartools::createUnitAnimations(std::type_index correspondingType, int iconSquareWidth_px, int spriteCountMoving, int spriteCountAttacking, int spriteCountDying) {
	TextureMap& textMap = m_mapUnitTexturesHuman; // Does not matter whether orc or human, will be changed individually, e.g., in getUnitIdleAnimation
	PlayerColor color = PlayerColor::Red; // Color does not matter, will be changed individually, e.g., in getUnitIdleAnimation

	Texture* pTexture = textMap[correspondingType][color];
	assert(pTexture);


	// Mage,Footman,ARcher
	// ToDo: already in direction.h?
	std::vector<Direction> vecDirections = { Direction::N, Direction::NE, Direction::E, Direction::SE, Direction::S, Direction::SW, Direction::W, Direction::NW };
	for (Direction direction : vecDirections) {
		// Do we have to flip the sprites?
		int colOffset = (int)direction;
		bool flip = false;
		if (colOffset > 4) {
			colOffset -= (colOffset - 4) * 2;
			flip = true;
		}

		// Do we have to adjust the position as the unit sprite is actually larger than the available tile area?
		Point posOffset = { 0, 0 };
		int unitSize_tile = gMapUnitTilesize[correspondingType];

		if (iconSquareWidth_px != Tile::TileWidth_px * unitSize_tile) {
			int offset_CenterPoint_px = iconSquareWidth_px / 2 - 16 * unitSize_tile;
			posOffset.x = -offset_CenterPoint_px;
			posOffset.y = -offset_CenterPoint_px;
		}

		// Movement animations
		unsigned int interval_ms = 100;
		Animation anim(textMap[correspondingType][color], interval_ms);

		int startOffset = 0;
		for (int rowOffset = startOffset; rowOffset < startOffset + spriteCountMoving; ++rowOffset) {
			Rect r = { colOffset * iconSquareWidth_px, rowOffset * iconSquareWidth_px, iconSquareWidth_px, iconSquareWidth_px };
			anim.addSprite(Sprite(anim.getBaseTexture(), r, posOffset, flip));
		}

		mapUnitMovementAnimations[correspondingType][direction] = anim;



		// Attack animations
		startOffset += spriteCountMoving;
		// spriteCountAttacking
		//  mapUnitAttackAnimations


		// Dying animations
		// interval_ms = 250;


		if (correspondingType == typeid(Ballista)) {
			// Catapults and Ballistas simply explode, they do not have additional sprites
			mapUnitDyingAnimations[correspondingType][direction] = getExplosionAnimation();
			continue;
		}

		// Add unit specific sprites
		startOffset += spriteCountAttacking;
		Animation animDying(textMap[correspondingType][color], interval_ms, true);

		// startOffset = 9;

		for (int rowOffset = startOffset; rowOffset < startOffset + spriteCountDying; ++rowOffset) {
			Rect r = { colOffset * iconSquareWidth_px, rowOffset * iconSquareWidth_px, iconSquareWidth_px, iconSquareWidth_px };
			animDying.addSprite(Sprite(animDying.getBaseTexture(), r, posOffset, flip));
		}

		// Add generic sprites from corpses.png
		//     Texture *pTextureCorpses = ;
		/*

		startOffset = 0;
		for (int rowOffset = startOffset; rowOffset < startOffset + 6; ++rowOffset) {
			Rect r = { colOffset * iconSquareWidth_px, rowOffset * iconSquareWidth_px, iconSquareWidth_px, iconSquareWidth_px };
			animDying.addSprite(Sprite(&textMap[typeid(Peasant)][color], r, posOffset, flip));
		}
		*/
		// 0,1 human, orc
		// 2,3,4,5

		mapUnitDyingAnimations[correspondingType][direction] = animDying;
	}
}


void ResourceManager_wartools::createVesselAnimations(std::type_index correspondingType, int iconSquareWidth_px) {
	TextureMap& textMap = m_mapUnitTexturesHuman; // Does not matter whether orc or human, will be changed individually, e.g., in getUnitIdleAnimation
	PlayerColor color = PlayerColor::Red; // Color does not matter, will be changed individually, e.g., in getUnitIdleAnimation

	Texture* pTexture = textMap[correspondingType][color];
	assert(pTexture);


	// In direction.h?
	std::vector<Direction> vecDirections = { Direction::N, Direction::NE, Direction::E, Direction::SE, Direction::S, Direction::SW, Direction::W, Direction::NW };
	for (Direction direction : vecDirections) {
		// Do we have to flip the sprites?
		int colOffset = (int)direction;
		bool flip = false;
		if (colOffset > 4) {
			colOffset -= (colOffset - 4) * 2;
			flip = true;
		}

		// Do we have to adjust the position as the unit sprite is actually larger than the available tile area?
		Point posOffset = { 0, 0 };
		int unitSize_tile = gMapUnitTilesize[correspondingType];

		if (iconSquareWidth_px != Tile::TileWidth_px * unitSize_tile) {
			int offset_CenterPoint_px = iconSquareWidth_px / 2 - 16 * unitSize_tile;
			posOffset.x = -offset_CenterPoint_px;
			posOffset.y = -offset_CenterPoint_px;
		}

		// Movement animations
		unsigned int interval_ms = 1500;
		Animation anim(textMap[correspondingType][color], interval_ms);

		Rect r = { colOffset * iconSquareWidth_px, 0, iconSquareWidth_px, iconSquareWidth_px };
		anim.addSprite(Sprite(anim.getBaseTexture(), r, posOffset, flip));


		posOffset.y += 3; // Ships float up and down when being idle
		anim.addSprite(Sprite(anim.getBaseTexture(), r, posOffset, flip));

		mapUnitMovementAnimations[correspondingType][direction] = anim;


		// Sinking animations
		posOffset.x = 0;
		posOffset.y = 0;
		Animation animSinking(textMap[correspondingType][color], interval_ms, true);

		// Turtle and GnomishSubmarine just have bubbles when sinking
		if (correspondingType != typeid(GnomishSubmarine)) {
			Rect rectSinking = { colOffset * iconSquareWidth_px, 1 * iconSquareWidth_px, iconSquareWidth_px, iconSquareWidth_px };
			animSinking.addSprite(Sprite(anim.getBaseTexture(), rectSinking, posOffset, flip));

			rectSinking.y = 2 * iconSquareWidth_px;
			animSinking.addSprite(Sprite(anim.getBaseTexture(), rectSinking, posOffset, flip));
		}

		// Generic sinking part (bubbles in water)
		int bubbleWidth_px = 504 / 7;
		Rect rectBubbles = { 0, 6 * bubbleWidth_px, bubbleWidth_px, bubbleWidth_px };

		// Bubbles are part of the neutral/units/corpses.png file. Bubbles do not have a player specific color meaning "Red" ist not important here
		animSinking.addSprite(Sprite(&m_mapUnitTexturesNeutral[typeid(Peasant)][PlayerColor::Red], rectBubbles));

		mapUnitDyingAnimations[correspondingType][direction] = animSinking;
	}
}


Animation ResourceManager_wartools::getBuildingAnimation(Building* pBuilding, AnimationType animType) {
	return 0;
}


Animation ResourceManager_wartools::getUnitAnimation(Unit* pUnit, Direction direction, AnimationType animType) {
	//  std::cerr << "Typeid: " << typeid(*pUnit).name() << std::endl;
	assert(mapUnitMovementAnimations.contains(typeid(*pUnit))); // Unit type considered?
	assert(mapUnitMovementAnimations[typeid(*pUnit)].contains(direction)); // Appropriate direction considered?

	// Use correct texture map depending on the unit's fraction
	TextureMap& textMap = (pUnit->getFraction() == Fraction::Orc) ? m_mapUnitTexturesOrc : m_mapUnitTexturesHuman;

	// Get underlaying base texture, e.g., "giant_turtle.png" or "knight.png"
	// with correct color 
	// Nicht immer korrekt: z.b. wenn eine neutrale einheit übernommen wurde. Oder dann sogar automatisch korrekt?!
	Texture* pCorrectColorTexture = textMap[typeid(*pUnit)][pUnit->getOwner()->getColor()];
	assert(pCorrectColorTexture);


	if (animType == AnimationType::Moving) {
		// Get appropriate animation
		Animation a(mapUnitMovementAnimations[typeid(*pUnit)][direction]);
		// Update general animation with specific colored unit texture
		a.setBaseTexture(pCorrectColorTexture);
		return a;
	}

	if (animType == AnimationType::Dying) {
		// Get appropriate animation
		Animation a(mapUnitDyingAnimations[typeid(*pUnit)][direction]);
		// Update general animation with specific colored unit texture
		a.setBaseTexture(pCorrectColorTexture);
		return a;
	}

	throw std::logic_error("AnimationType not yet implemented!");
}

/*
 * Returns an Animation object with just one sprite.
 */
Sprite ResourceManager_wartools::getCritterSprite(Direction direction, bool dead) {
	assert(gMapUnitTilesize.contains(typeid(Critter)));

	int tile_size = gMapUnitTilesize[typeid(Critter)];

	int rowOffset = !dead ? 0 : 1;
	int colOffset = (int)direction;

	// See Direction.h
	// N = 0, NE  1, E  2, SE 3, S 4, SW 5, W 6, NW 7,
	// Each *.png has animations from N to S
	// The following sprites are created by flipping existing directions:
	// 
	//      SW = SE flipped   5  maps to 3    
	//      W = E flipped     6  maps to 2   
	//      NW = NE flipped   7  maps to 1   
	bool flip = false;
	if (colOffset > 4) {
		colOffset -= (colOffset - 4) * 2;
		flip = true;
	}

	// startoffset
	int iconWidth_px = tile_size * Tile::TileWidth_px;
	int iconHeight_px = tile_size * Tile::TileWidth_px;

	Rect rect = { colOffset * iconWidth_px, rowOffset * iconHeight_px, iconWidth_px, iconHeight_px };
	return Sprite(&m_pActiveCritterTexture, rect, flip);
}


Animation ResourceManager_wartools::getUnitIdleAnimation(Unit* pUnit, Direction direction) {
	if (pUnit->isSeaObject()) {
		assert(mapUnitMovementAnimations.contains(typeid(*pUnit))); // Unit type considered?
		assert(mapUnitMovementAnimations[typeid(*pUnit)].contains(direction)); // Appropriate direction considered?
	}
	else {
		assert(mapIdleAnimations.contains(typeid(*pUnit))); // Unit type considered?
		assert(mapIdleAnimations[typeid(*pUnit)].contains(direction)); // Appropriate direction considered?
	}

	// Get appropriate animation
	// Ships idle animations are equal to the movement animations (just a single image, floating up and down)
	Animation a(pUnit->isSeaObject() ? mapUnitMovementAnimations[typeid(*pUnit)][direction] : mapIdleAnimations[typeid(*pUnit)][direction]);

	// Use correct texture map depending on the unit's fraction
	TextureMap& textMap = (pUnit->getFraction() == Fraction::Orc) ? m_mapUnitTexturesOrc : m_mapUnitTexturesHuman;

	/*
	 * Get underlaying base texture, e.g., "giant_turtle.png" or "knight.png"
	 * with correct color
	 */
	 // ToDo: Correct color if a neutral unit is freed?
	Texture* pCorrectColorTexture = textMap[typeid(*pUnit)][pUnit->getOwner()->getColor()];
	assert(pCorrectColorTexture);

	// Update general animation with specific colored unit texture
	a.setBaseTexture(pCorrectColorTexture);
	return a;
}


Animation ResourceManager_wartools::getDyingAnimation(Unit* pUnit) {
	/*
	Rect rect = { 0, 0, squareWidth_px, squareWidth_px };


	Animation anim(textMap[correspondingType][color], 3000);

	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			anim.addSprite(Sprite(anim.getBaseTexture(), rect, posOffset));  // Reihenfolge stimmt evtl. nicht
			rect.y += squareWidth_px;
		}

		rect.x += squareWidth_px;
	}

	mapIdleAnimations[correspondingType][Direction::N] = anim;
	*/
	return 0;
}



Sprite* ResourceManager_wartools::getUIElement(Ressources uiElement) {
	return 0;
}

#include <cmath>

void ResourceManager_wartools::createidleAnimation(std::type_index correspondingType, int squareWidth_px, int rows, int cols, unsigned int interval_ms, Point posOffset = { 0, 0 }) {
	assert(rows > 0);
	assert(cols > 0);

	TextureMap& textMap = m_mapUnitTexturesHuman; // Does not matter whether orc or human, will be changed individually, e.g., in getUnitIdleAnimation
	PlayerColor color = PlayerColor::Red; // Color does not matter, will be changed individually, e.g., in getUnitIdleAnimation

	Texture* pTexture = textMap[correspondingType][color];
	assert(pTexture);


	// Offset calculation. In most cases we have to adjust the initial draw position because
	// almost none unit has 32x32 sprites which would fit exactly to a 32x32 tile (a critter does)
	//
	// Example: Elven Archer has a dimension of 360px/5 = 64px (see *.png files). Hence, we have
	// to shift x and y coordinates accordingly such that the center of the sprite matches the center of the tile.
	//
	// Moreover it might be the case that a unit does occupy more than one tile (dragons, ships, ...)
	// In these cases we have to shift even more.
	int tile_size = gMapUnitTilesize[correspondingType];

	if (squareWidth_px != Tile::TileWidth_px * tile_size) {
		int offset_MiddlePoint_px = squareWidth_px / 2 - 16 * tile_size;
		posOffset.x = -offset_MiddlePoint_px;
		posOffset.y = -offset_MiddlePoint_px;
	}

	Rect rect = { 0, 0, squareWidth_px, squareWidth_px };

	Animation anim(textMap[correspondingType][color], interval_ms);

	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			anim.addSprite(Sprite(anim.getBaseTexture(), rect, posOffset));  // ToDo: Ordering maybe wrong?
			rect.y += squareWidth_px;
		}

		rect.x += squareWidth_px;
	}

	mapIdleAnimations[correspondingType][Direction::N] = anim;
}


void ResourceManager_wartools::createExplosionAnimation() {
	int iconSquareWidth_px = 320 / 5;
	int spriteCnt = 16;

	Texture* pTexture = new Texture("data/graphics/missiles/explosion.png", m_pRenderer);

	Point posOffset = { -16, -16 };

	int interval_ms = 100;
	Animation anim(pTexture, interval_ms, true);


	int extractedSprites = 0;
	int rowOffset = 0;

	while (extractedSprites < spriteCnt) {
		for (int colOffset = 0; colOffset < 5; ++colOffset) {
			Rect r = { colOffset * iconSquareWidth_px, rowOffset * iconSquareWidth_px, iconSquareWidth_px, iconSquareWidth_px };
			anim.addSprite(Sprite(anim.getBaseTexture(), r, posOffset));

			if (++extractedSprites >= spriteCnt)
				break;
		}

		++rowOffset;
	}

	m_animExplosion = anim;
}


void ResourceManager_wartools::loadAndStoreDestroyedBuildingTexture(const std::string& basePath, BuildingDestroyedType type, Season season) {
	std::string fullPath = basePath + (type == BuildingDestroyedType::DestroyedBig ? "/destroyed_site.png" : "/small_destroyed_site.png");

	SDL_Surface* pSurface = IMG_Load(fullPath.c_str());
	if (!pSurface)
		assert(0);

	m_mapDestroyedSites[type][season] = new Texture(pSurface, m_pRenderer, false);
	SDL_FreeSurface(pSurface);
}


void ResourceManager_wartools::createDestroyedSiteAnimations() {
	const std::string basePaths[] = { "data/graphics/tilesets/summer/neutral/buildings",
									 "data/graphics/tilesets/swamp/neutral/buildings",
									 "data/graphics/tilesets/wasteland/neutral/buildings",
									 "data/graphics/tilesets/winter/neutral/buildings" };
	const Season seasons[] = { Season::Summer, Season::Swamp, Season::Wasteland, Season::Winter };

	for (const auto& basePath : basePaths) {
		for (const auto& season : seasons) {
			loadAndStoreDestroyedBuildingTexture(basePath, BuildingDestroyedType::DestroyedBig, season);
			loadAndStoreDestroyedBuildingTexture(basePath, BuildingDestroyedType::DestroyedSmall, season);
		}
	}


	Point posOffset = { 0,0 };

	int interval_ms = 5000;


	Texture** pTextureActiveDestroyedBuildingTexture = &m_mapDestroyedSites[BuildingDestroyedType::DestroyedBig][Season::Summer];

	{
		Animation anim(*pTextureActiveDestroyedBuildingTexture, interval_ms, true);

		Rect size = { 0, 0, 63, 63 };
		anim.addSprite(Sprite(pTextureActiveDestroyedBuildingTexture, size));

		size.y = 64;
		anim.addSprite(Sprite(pTextureActiveDestroyedBuildingTexture, size));

		m_animDestroyedBuilding = anim;
	}

	{
		Animation anim(*pTextureActiveDestroyedBuildingTexture, interval_ms, true);

		Rect size = { 0, 128, 63, 63 };
		anim.addSprite(Sprite(pTextureActiveDestroyedBuildingTexture, size));

		size.y += 64;
		anim.addSprite(Sprite(pTextureActiveDestroyedBuildingTexture, size));

		m_animDestroyedBuildingSea = anim;
	}
}



void ResourceManager_wartools::createRuneAnimation() {
	SDL_Surface* pSurface = IMG_Load((strPrefix + "missiles" + PATH_SEPARATOR + "rune.png").c_str());
	assert(pSurface);

	Texture* pTexRune = new Texture(pSurface, m_pRenderer, false);
	SDL_free(pSurface);


	const int squareWidth_px = 16;
	Rect rect = { 0, 0, squareWidth_px, squareWidth_px };

	Animation anim(pTexRune, 150);

	Point posOffset = { 8, 8 };

	for (int r = 0; r < 4; ++r) {
		anim.addSprite(Sprite(anim.getBaseTexture(), rect, posOffset));  // Reihenfolge stimmt evtl. nicht
		rect.y += squareWidth_px;
	}

	m_runeAnimation = anim;
}


void ResourceManager_wartools::createBuffIcons() {
	std::string strPath = strPrefix + "ui" + PATH_SEPARATOR + "bloodlust,haste,slow,invisible,shield.png";
	SDL_Surface* pSurface = IMG_Load(strPath.c_str());
	assert(pSurface);

	pBuffIcons = new Texture(pSurface, m_pRenderer, false);
	SDL_free(pSurface);

	const int squareWidth_px = 16;

	Rect rect = { 0, 0, squareWidth_px, squareWidth_px };
	m_mapBuffSprites[BuffType::Bloodlust] = new Sprite(&pBuffIcons, rect);

	rect.y += 16;
	m_mapBuffSprites[BuffType::Haste] = new Sprite(&pBuffIcons, rect);


	rect.y += 16;
	m_mapBuffSprites[BuffType::Slow] = new Sprite(&pBuffIcons, rect);

	rect.y += 16;
	m_mapBuffSprites[BuffType::Invisibility] = new Sprite(&pBuffIcons, rect);


	rect.y += 16;
	m_mapBuffSprites[BuffType::UnholyArmor] = new Sprite(&pBuffIcons, rect);
}


void ResourceManager_wartools::createidleAnimations() {
	// See png files for row/column structure
	//   createidleAnimation(typeid(Skeleton), 280 / 5, 1, 1, 99999);  // ToDo: Internal structure differs completely from other units...

	createidleAnimation(typeid(Ballista), 320 / 5, 1, 1, 10000); // todo fix the 10000
	createidleAnimation(typeid(Dwarfes), 280 / 5, 1, 1, 10000);
	createidleAnimation(typeid(Archer), 360 / 5, 1, 1, 10000);
	createidleAnimation(typeid(Footman), 360 / 5, 1, 1, 10000);
	createidleAnimation(typeid(GnomishFlyingMaschine), 400 / 5, 1, 1, 10000);
	createidleAnimation(typeid(GryphonRider), 400 / 5, 1, 1, 10000);
	createidleAnimation(typeid(Knight), 360 / 5, 1, 1, 10000);
	createidleAnimation(typeid(Mage), 360 / 5, 1, 1, 10000);
	createidleAnimation(typeid(Peasant), 360 / 5, 1, 1, 10000);




	createVesselAnimations(typeid(Battleship), 440 / 5);
	createVesselAnimations(typeid(Destroyer), 400 / 5);
	createVesselAnimations(typeid(GnomishSubmarine), 360 / 5);
	createVesselAnimations(typeid(Tanker), 360 / 5);
	createVesselAnimations(typeid(Transport), 360 / 5);


	//   createUnitAnimations(typeid(Skeleton), 360 / 5, 5, 5, 4);  // ToDo: Internal structure differs completely from other units...
	createUnitAnimations(typeid(Archer), 360 / 5, 5, 2, 3);

	// Again, internal structure differs completely from other units...
	createUnitAnimations(typeid(Dwarfes), 280 / 5, 2, 3, 3);

	createUnitAnimations(typeid(Ballista), 320 / 5, 2, 2, 0);
	createUnitAnimations(typeid(GnomishFlyingMaschine), 400 / 5, 2, 0, 2);
	createUnitAnimations(typeid(GryphonRider), 400 / 5, 4, 3, 5);
	createUnitAnimations(typeid(Footman), 360 / 5, 5, 4, 3);
	createUnitAnimations(typeid(Knight), 360 / 5, 5, 4, 5);
	createUnitAnimations(typeid(Mage), 360 / 5, 5, 4, 7);
	createUnitAnimations(typeid(Peasant), 360 / 5, 5, 5, 3);


	// CannonBall wird größer in 2 stufen
	// FlameShield zirkuliert um Einheit
	// Blizzard, DeathAndDecay: Verteilen sich auf mehrere Felder
	enum VariousSprites { Arrow, Axe, BallistaBolt, CatapultRock, CannonBall, SubmarineMissile };
	enum VariousAnimations { Explosion, Heal, Tornado, Rune, FlameShield, Blizzard, DeathAndDecay };
	enum VariousAnimationsWithDirection { DragonBreath, GryphonHammer, Lightning };





	//  mapUnitMovementAnimations[correspondingType][direction] = anim;
						  /*
	  {"oil_tanker_empty.png", typeid(Tanker)},         // Problem!!!!
	  {"oil_tanker_full.png", typeid(Tanker)},           // Problem!!!!
	  {"peasant.png", typeid(Peasant)},                 // Problem!!!!
	  {"peasant_with_gold.png", typeid(Peasant)},       // Problem!!!!
	  {"peasant_with_wood.png", typeid(Peasant)},       // Problem!!!!
	{"transport.png", typeid(Transport)},
						*/

						/*

							{ // GnomishFlyingMaschine
								std::type_index correspondingType= typeid(GnomishFlyingMaschine);
								Animation anim(textMap[correspondingType][color], 100);

								Rect r = {0, 0, 80, 80};

								for(int i=0; i<2; ++i) {
									anim.addSprite(new Sprite(anim.getBaseTexture(), r));  // Reihenfolge stimmt evtl. nicht
									r.y += 80;
								}

								mapIdleAnimations[correspondingType][Direction::N] = anim;
							}
						*//*

							{
								std::type_index correspondingType = typeid(Battleship);
								Animation anim(textMap[correspondingType][color], 1000);


								Rect r = {0, 0, 88, 88};
								anim.addSprite(new Sprite(anim.getBaseTexture(), r));


								// todo: alle einzeln aktualisieren
								// oder
								// der pointer auf pointer ansatz

								Point posOffset = {0, 3}; // Floats up and down when being idle
								anim.addSprite(new Sprite(anim.getBaseTexture(), r, posOffset));
								m_idleBattleShip = anim;
							}*/
}

#include "Buildings/Building.h"

Sprite ResourceManager_wartools::getNeutralBuildingSprite(Building* pBuilding, int index) {
	auto& type = typeid(*pBuilding);
	assert(m_mapNeutralBuildingTextures.contains(type));
	assert(m_mapNeutralBuildingTextures[type].contains(m_currentSeason));

	// getNeutralBuildingSprite & getBuildingSprite -> funktion draus machen

	// Get side length for building (e.g., 64, 96, 128, ...)
	Texture* pTexture = m_mapNeutralBuildingTextures[type][m_currentSeason];
	Rect texDim = { 0,0,0,0 };
	SDL_QueryTexture(pTexture->getSDLTexture(), nullptr, nullptr, &texDim.w, &texDim.h);

	// Buildings sprites are always squared -> width = height
	Rect rect = { 0, 0, texDim.w, texDim.w };

	if (index) {
		// Change y offset
		rect.y = index * texDim.w;
		assert(rect.y <= texDim.h);
	}

	return Sprite(&m_mapNeutralBuildingTextures[type][m_currentSeason], rect);
}


Sprite ResourceManager_wartools::getBuildingSprite(Building* pBuilding, Fraction fraction, PlayerColor color, bool underConstruction) {
	std::unordered_map<std::type_index, std::unordered_map<PlayerColor, Texture*>>& texMap = (fraction == Fraction::Orc) ? m_ActiveSeasonBuildingsOrc : m_ActiveSeasonBuildingsHuman;

	// Active above map when orc textures ready        
	//BuildingTextureMap& texMap = m_mapBuildingTexturesHuman;

	// Type available in map?
	auto& type = typeid(*pBuilding);
	//  assert(texMap.contains(type) || );

	  // Playercolor available in map?
	//  assert(texMap[type].contains(color));

	  // Access granted...

	  /*
	  if(pTH && pTH->isKeepOrUpgradingTo()) return m_mapBuildingTextures_Orc_Level1[pOwner->getColor()][typeId][m_pud_Era][id];
	  if(pTH && pTH->isCastleOrUpgradingTo()) return m_mapBuildingTextures_Orc_Level2[pOwner->getColor()][typeId][m_pud_Era][id];

	  if(pST && pST->isGuardTowerOrUpgradingTo()) return m_mapBuildingTextures_Orc_Level1[pOwner->getColor()][typeId][m_pud_Era][id];
	  if(pST && pST->isCannonTowerOrUpgradingTo()) return m_mapBuildingTextures_Orc_Level2[pOwner->getColor()][typeId][m_pud_Era][id];
	  */

	  /*
	   * Buildings have typically two sprites which are stacked:
	   *
	   * The fininished building. (y offset in pixel = 0)
	   * Building under construction. (y offset in pixel = 1 time building side length)
	   *
	   * And, special case, oil rig: Oil Rig in use. (y offset in pixel = 2 times building side length)
	   */


	Townhall* pTownHall = dynamic_cast<Townhall*>(pBuilding);
	ScoutTower* pScoutTower = dynamic_cast<ScoutTower*>(pBuilding);

	Texture** pTexture = nullptr;

	// Fixme -> underConstruction
	if (pTownHall) {
		Townhall::UpgradeLevel ul = pTownHall->getUpgradeLevel();

		if (ul == Townhall::UpgradeLevel::UpgradingToKeep) {
			underConstruction = true;
			ul = Townhall::UpgradeLevel::Keep; // Take the upgrade sprite from the keep png file
		}
		else if (ul == Townhall::UpgradeLevel::UpgradingToCastle) {
			underConstruction = true;
			ul = Townhall::UpgradeLevel::Castle; // Take the upgrade sprite from the castle png file
		}

		auto& townhallTexTargetMap = pTownHall->getFraction() == Fraction::Alliance ? m_ActiveSeasonTownhallHuman : m_ActiveSeasonTownhallOrc;
		pTexture = &townhallTexTargetMap[ul][color];
	}
	else if (pScoutTower) {
		ScoutTower::UpgradeLevel ul = pScoutTower->getUpgradeLevel();

		if (ul == ScoutTower::UpgradeLevel::UpgradingToGuardTower || ul == ScoutTower::UpgradeLevel::UpgradingToCannonTower) {
			underConstruction = true; // underConstruction ist noch was anderes als "upgrading"
			ul = ScoutTower::UpgradeLevel::GuardTower; // Upgrade sprite is the same for both guard and cannon tower. We take it from the guard tower png file
		}

		auto& scoutTowerTexTargetMap = pScoutTower->getFraction() == Fraction::Alliance ? m_ActiveSeasonScoutTowerTexturesHuman : m_ActiveSeasonScoutTowerTexturesOrc;
		pTexture = &scoutTowerTexTargetMap[ul][color];
	}
	else {
		pTexture = &texMap[type][color]; // ?! todo: activeTextMap anstatt texmap
	}



	// Get side length for building (e.g., 64, 96, 128, ...)
	Rect texDim = { 0,0,0,0 };
	SDL_QueryTexture((*pTexture)->getSDLTexture(), nullptr, nullptr, &texDim.w, &texDim.h);

	// Buildings sprites are always squared -> width = height
	Rect rect = { 0, 0, texDim.w, texDim.w };

	if (underConstruction) {  
		// Change y offset
		rect.y = texDim.w;
	}

	return Sprite(pTexture, rect);
}




std::unordered_map<Season, std::string> mapSeasonToPath = {
	{Season::Summer, "summer/"},
	{Season::Swamp, "swamp/"},
	{Season::Wasteland, "wasteland/"},
	{Season::Winter, "winter/"}
};


void ResourceManager_wartools::readBuildings(bool orcs) {
	std::string strToFolder("data/graphics/tilesets/");

	// ToDo: consider all...
	std::vector<Season> seasons = { Season::Summer, Season::Swamp, Season::Wasteland, Season::Winter };
	for (Season season : seasons) {
		std::string strPathToSeasonFolder(strToFolder);
		strPathToSeasonFolder += mapSeasonToPath[season];

		for (const auto& [key_imgFileName, value] : mapPNGFileToBuilding) {
			std::string tmp = strPathToSeasonFolder + "human/buildings/" + key_imgFileName;
			readBuildingSprite(season, tmp, false);
		}

		for (const auto& [key_imgFileName, value] : mapPNGFileToBuilding_Orc) {
			std::string tmp = strPathToSeasonFolder + "orc/buildings/" + key_imgFileName;
			readBuildingSprite(season, tmp, true);
		}


		for (const auto& [key_imgFileName, value] : mapPNGFileToBuilding_Neutral) {
			std::string tmp = strPathToSeasonFolder + "neutral/buildings/" + key_imgFileName;

			SDL_Surface* pSurface = IMG_Load(tmp.c_str());
			if (!pSurface)
				continue; // return

			// ToDo: Extract method
			std::string pngPath(tmp);
			std::size_t posSlash = tmp.find_last_of('/');
			pngPath = pngPath.erase(0, posSlash + 1);

			std::type_index correspondingType = mapPNGFileToBuilding_Neutral.at(pngPath);
			m_mapNeutralBuildingTextures[correspondingType][season] = new Texture(pSurface, m_pRenderer, false);
			SDL_FreeSurface(pSurface);
		}
	}
}


Sprite* ResourceManager_wartools::getTileSprite(int tile_id) {
	assert(m_pActiveTileSet); // Required tileset texture available?

	// tile_id and corresponding season exist?
	if (!m_mapPUDId_to_TileSetIndex.contains(tile_id)) {
		std::stringstream ss;
		ss << "Requested tile_id can not be mapped: ";
		ss << std::dec << tile_id << "(0x" << std::hex << ")";

		throw std::logic_error(ss.str());
	}

	// ToDo: Exception
	assert(m_mapPUDId_to_TileSetIndex[tile_id].contains(m_currentSeason));

	// Just create one sprite for a tile_id, not several!
	if (!m_tileLookUp.contains(tile_id)) {
		int val = m_mapPUDId_to_TileSetIndex[tile_id][m_currentSeason];
		int rowOffset = val / 16;
		int colOffset = val % 16;

		int iconWidth_px = 32;
		int iconHeight_px = 32;
		Rect rect = { colOffset * iconWidth_px, rowOffset * iconHeight_px, iconWidth_px, iconHeight_px };

		m_tileLookUp.insert_or_assign(tile_id, Sprite(&m_pActiveTileSet, rect));
	}

	return &m_tileLookUp.at(tile_id);
}


void ResourceManager_wartools::readBuildingSprite(Season season, const std::string& pathToFile, bool orcs) {
	SDL_Surface* pSurface = IMG_Load(pathToFile.c_str());
	if (!pSurface)
		return;

	assert(pSurface);

	// ToDo: Extract method
	std::string pngPath(pathToFile);
	std::size_t posSlash = pathToFile.find_last_of('/'); // ToDo: use PATH_SEPARATOR
	pngPath = pngPath.erase(0, posSlash + 1);

	BuildingTextureMap& texTargetMap = orcs ? m_mapBuildingTexturesOrc : m_mapBuildingTexturesHuman;

	MapFileNameToTypeID& mapFileNameToTypeID = orcs ? mapPNGFileToBuilding_Orc : mapPNGFileToBuilding;


	for (auto const& [color, palette] : mapPlayerPaletteColors) {
		SDL_SetPaletteColors(pSurface->format->palette, &palette[0], 208, 4);

		std::type_index correspondingType = mapFileNameToTypeID.at(pngPath);
		Texture* pTexture = new Texture(pSurface, m_pRenderer, false);

		// Special case... TownHall, Keep and Castle all map to the same class... (ugly, I know)
		// typedef std::unordered_map<Townhall::, std::unordered_map<Season, std::unordered_map<PlayerColor, Texture*>>> TownhallTextureMap;
		if (correspondingType == typeid(Townhall)) {
			auto& townhallTexTargetMap = orcs ? m_mapTownhallTexturesOrc : m_mapTownhallTexturesHuman;
			Townhall::UpgradeLevel ul = Townhall::UpgradeLevel::Townhall;

			if (pngPath == "keep.png" || pngPath == "stronghold.png") {
				ul = Townhall::UpgradeLevel::Keep;
			}
			else if (pngPath == "castle.png" || pngPath == "fortress.png") {
				ul = Townhall::UpgradeLevel::Castle;
			}

			townhallTexTargetMap[season][ul][color] = pTexture;
			continue;
		}
		else if (correspondingType == typeid(ScoutTower)) {
			auto& scoutTowerTexTargetMap = orcs ? m_mapScoutTowerTexturesOrc : m_mapScoutTowerTexturesHuman;
			ScoutTower::UpgradeLevel ul = ScoutTower::UpgradeLevel::ScoutTower;

			if (pngPath == "guard_tower.png") {
				ul = ScoutTower::UpgradeLevel::GuardTower;
			}
			else if (pngPath == "cannon_tower.png") {
				ul = ScoutTower::UpgradeLevel::CannonTower;
			}
			else if (pngPath == "scout_tower.png") {
				ul = ScoutTower::UpgradeLevel::ScoutTower;
			}

			scoutTowerTexTargetMap[season][ul][color] = pTexture;
			continue;
		}

		texTargetMap[correspondingType][season][color] = pTexture;
	}

	SDL_FreeSurface(pSurface);
}


// Neutral::  daemon.png, skeleton.png, corpses.png
void ResourceManager_wartools::createUnitSprites(Fraction fraction) {
	std::string strToFolder("data/graphics/");

	TextureMap* textMap = nullptr;

	switch (fraction) {
	case Fraction::Alliance:
		strToFolder += "human/units";
		textMap = &m_mapUnitTexturesHuman;
		break;
	case Fraction::Orc:
		strToFolder += "orc/units";
		textMap = &m_mapUnitTexturesOrc;
		break;
	case Fraction::Neutral:
		strToFolder += "neutral/units";
		textMap = &m_mapUnitTexturesNeutral;
		break;
	}


	for (const auto& entry : fs::directory_iterator(strToFolder)) {
		if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".png") {
			// ToDo: Extract method
			std::string pngPath(entry.path().string());
			std::size_t posSlash = entry.path().string().find_last_of(PATH_SEPARATOR);
			pngPath = pngPath.erase(0, posSlash + 1);
			std::cout << "\t\t\t pngPath: " << pngPath << std::endl;

			SDL_Surface* pSurface = IMG_Load(entry.path().string().c_str());  // ToDo: Delete pSurface after creating Texture?
			assert(pSurface);

			// Todo: Extract method?
			for (auto const& [color, palette] : mapPlayerPaletteColors) {
				SDL_SetPaletteColors(pSurface->format->palette, &palette[0], 208, 4);

				std::type_index correspondingType = mapPNGFileToType.at(pngPath);
				(*textMap)[correspondingType][color] = new Texture(pSurface, m_pRenderer, false);
			}
		}
	}
}


TileSetContainer::TileSetContainer(Renderer* pRenderer) {
	pIcons = new Texture("data/graphics/tilesets/summer/terrain/summer.png", pRenderer);


	for (const auto& entry : fs::directory_iterator("data/graphics/tilesets/summer/human/buildings")) {
		if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".png") {
			std::string pngPath(entry.path().string());
			std::size_t posSlash = entry.path().string().find_last_of(PATH_SEPARATOR);
			pngPath = pngPath.erase(0, posSlash + 1);
			//std::cout << "\t\t: "<< entry.path().string().erase(0,entry.path().string().find_last_of(PATH_SEPARATOR)+1) << std::endl;


			SDL_Surface* pSurface = IMG_Load(entry.path().string().c_str());
			assert(pSurface);




			/* Each building *.png has at least two Sprites: Finished and under construction.
			 * The oilrig is special: It has a third Sprite when visited by an oiltanker
			 */
			int cntSprites = pSurface->h / pSurface->w;
			int spriteHeight = pSurface->h / cntSprites;


			SDL_SetPaletteColors(pSurface->format->palette, &violetPaletteColors[0], 208, 4);

			mapTextures[pngPath] = new Texture(pSurface, pRenderer);


			//buildingSprites.
			//new Sprite(&vecTextures.back(), r);
			//  mapTextures.back
		}
	}


}

std::unordered_map<std::string, Texture*> mapUIPngs;


Texture* pTextureFarm = nullptr;
ResourceManager_wartools::ResourceManager_wartools(const std::string& pathToGraphicsFolder, Renderer* pRenderer) : m_pRenderer(pRenderer) {
	if (pathToGraphicsFolder.rfind("titi", 0) != 0) {
		assert("Unexpected data folder prefix!");
	}


	loadCursors();


	//readDirectoryRecursively(pathToGraphicsFolder + "/ui");
	readIcons();
	readTileSetTextures();
	readCritters();
	readBuildings(false);
	//readBuilding(true);

	readMissiles();

	createRuneAnimation();
	createExplosionAnimation();
	createDestroyedSiteAnimations();

	createBuffIcons();

	createUnitSprites(Fraction::Alliance);
	createUnitSprites(Fraction::Orc);
	createUnitSprites(Fraction::Neutral);
	createidleAnimations();

	parseLUATileSetFile(Season::Summer, "data/tilesets/summer.lua");
	parseLUATileSetFile(Season::Winter, "data/tilesets/winter.lua");
	parseLUATileSetFile(Season::Wasteland, "data/tilesets/wasteland.lua");
	parseLUATileSetFile(Season::Swamp, "data/tilesets/swamp.lua");



	{
		SDL_Surface* pSurface = IMG_Load("data/graphics/neutral/buildings/land_construction_site.png");

		m_pTextureConstructionSites = new Texture(pSurface, m_pRenderer);
		Rect size = { 0, 0, 63, 63 };
		m_pSpriteConstructionSiteSmall = Sprite(&m_pTextureConstructionSites, size);

		size.y = 64;
		m_pSpriteConstructionSiteLarge = Sprite(&m_pTextureConstructionSites, size);
	}


	SDL_Surface* pSurface = IMG_Load("data/graphics/tilesets/summer/human/buildings/farm.png");

	/* For details about this magic (exact colors and the offset 208)
	 * refer to wargus-master/scrips/stratagus.lua and search for "DefinePlayerColorIndex(208, 4)"
	 *
	 *
	 * DefinePlayerColors({
  "red", {{164, 0, 0}, {124, 0, 0}, {92, 4, 0}, {68, 4, 0}},
  "blue", {{12, 72, 204}, {4, 40, 160}, {0, 20, 116}, {0, 4, 76}},
  "green", {{44, 180, 148}, {20, 132, 92}, {4, 84, 44}, {0, 40, 12}},
  "violet", {{152, 72, 176}, {116, 44, 132}, {80, 24, 88}, {44, 8, 44}},
  "orange", {{248, 140, 20}, {200, 96, 16}, {152, 60, 16}, {108, 32, 12}},
  "black", {{40, 40, 60}, {28, 28, 44}, {20, 20, 32}, {12, 12, 20}},
  "white", {{224, 224, 224}, {152, 152, 180}, {84, 84, 128}, {36, 40, 76}},
  "yellow", {{252, 252, 72}, {228, 204, 40}, {204, 160, 16}, {180, 116, 0}},
  "red", {{164, 0, 0}, {124, 0, 0}, {92, 4, 0}, {68, 4, 0}},
  "blue", {{12, 72, 204}, {4, 40, 160}, {0, 20, 116}, {0, 4, 76}},
  "green", {{44, 180, 148}, {20, 132, 92}, {4, 84, 44}, {0, 40, 12}},
  "violet", {{152, 72, 176}, {116, 44, 132}, {80, 24, 88}, {44, 8, 44}},
  "orange", {{248, 140, 20}, {200, 96, 16}, {152, 60, 16}, {108, 32, 12}},
  "black", {{40, 40, 60}, {28, 28, 44}, {20, 20, 32}, {12, 12, 20}},
  "white", {{224, 224, 224}, {152, 152, 180}, {84, 84, 128}, {36, 40, 76}},
  "yellow", {{252, 252, 72}, {228, 204, 40}, {204, 160, 16}, {180, 116, 0}},
})
	 */
	 //  std::vector<SDL_Color> sdlColors =  {{12, 72, 204, 255}, {4, 40, 160, 255}, {0, 20, 116, 255}, {0, 4, 76, 255}};
	if (!SDL_SetPaletteColors(pSurface->format->palette, &bluePaletteColors[0], 208, 4)) {
		std::cout << "PALETTE: " << SDL_GetError() << std::endl;
		printf("SDL_Init failed: %s\n", SDL_GetError());
	}




	pTextureFarm = new Texture(pSurface, m_pRenderer);
	Rect size = { 0,0,63,127 };
	pFarm = new Sprite(&pTextureFarm, size);
	int columns = 2;
	int iconWidth_px = 64;
	int iconHeight_px = 64;


	switchSeason(Season::Summer);
	// units [alliance][color]
	// buildings [tileset][alliance][color]
}


void ResourceManager_wartools::readMissiles() {
	std::string strToFolder("data/graphics/missiles/");

	m_mapMissiles[MissileType::Arrow] = new Texture(strToFolder + "arrow.png", m_pRenderer);
	m_mapMissiles[MissileType::Axe] = new Texture(strToFolder + "axe.png", m_pRenderer);
	m_mapMissiles[MissileType::BallistaBolt] = new Texture(strToFolder + "ballista_bolt.png", m_pRenderer);
	m_mapMissiles[MissileType::Cannon] = new Texture(strToFolder + "cannon.png", m_pRenderer);
	m_mapMissiles[MissileType::CatapultRock] = new Texture(strToFolder + "catapult_rock.png", m_pRenderer);
}


Sprite ResourceManager_wartools::getMissile(MissileType t) {
	assert(m_mapMissiles.contains(t));

	Rect dummy = { 0,0,32,32 };
	return Sprite(&m_mapMissiles.at(t), dummy);
}


Sprite ResourceManager_wartools::getBuildingConstructionSprite(int level) {
	assert(level == 1 || level == 2);
	assert(m_pTextureConstructionSites);

	return (level == 1) ? m_pSpriteConstructionSiteSmall : m_pSpriteConstructionSiteLarge;
}


Sprite ResourceManager_wartools::getUISprite(const std::string& strPngPath) {
	const std::string strPathUIFolder("data/graphics/ui/");
	std::string strPathToElement = strPathUIFolder + strPngPath;

	if (!mapUIPngs.contains(strPathToElement)) {
		mapUIPngs[strPathToElement] = new Texture(strPathToElement, m_pRenderer);
	}

	return Sprite(&mapUIPngs[strPathToElement]);
}


void ResourceManager_wartools::readDirectoryRecursively(const std::string& pathToGraphicsFolder) {
	for (const auto& entry : fs::directory_iterator(pathToGraphicsFolder)) {
		std::cout << entry.path() << " " << entry.is_directory() << " " << entry.is_regular_file() << " " << fs::path(entry.path()).extension() << std::endl;
		std::cout << "\t\t: " << entry.path().string().erase(0, strPrefix.size()) << std::endl;

		if (entry.is_directory()) {
			readDirectoryRecursively(entry.path().string());
		}
		else if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".png") {
			// new Texture(entry.path().string(), m_pRenderer);
		}
	}
}


const Sprite* ResourceManager_wartools::getBuildingIcon(int i) const {
	return 0;
}



// !! pointer copy?
std::vector<Sprite*> ResourceManager_wartools::extractSprites(Texture** pTexture, int cntRows, int cntCols, int iconWidth_px, int iconHeight_px) {
	std::vector<Sprite*> sprites;

	for (int r = 0; r < cntRows; ++r) {
		for (int c = 0; c < cntCols; ++c) {
			int offset_cols = c * iconWidth_px;
			int offset_rows = r * iconHeight_px;

			Rect size = { offset_cols, offset_rows, iconWidth_px, iconHeight_px };
			sprites.push_back(new Sprite(pTexture, size));
		}
	}

	return sprites;
}


void ResourceManager_wartools::switchBuildingSprites(Season s) {
	// Okay, this needs a bit of explanation. Wasteland actually uses many of the
	// summer sprites. As we just copy the pointers / color maps to the 'm_ActiveSeasonBuildingsHuman' (and orc) container,
	// we copy summer first and the few individual wasteland keys/values are replacing the some of the summer sprites afterwards
	if (s == Season::Wasteland)
		switchBuildingSprites(Season::Summer);

	for (const auto& [buildingType, seasonMap] : m_mapBuildingTexturesHuman) {
		if (seasonMap.contains(s))
			m_ActiveSeasonBuildingsHuman[buildingType] = seasonMap.at(s);
	}

	for (const auto& [buildingType, seasonMap] : m_mapBuildingTexturesOrc) {
		if (seasonMap.contains(s))
			m_ActiveSeasonBuildingsOrc[buildingType] = seasonMap.at(s);
	}


	// Townhall and ScoutTower are special, as 3 different *.pngs map to the same typeid -> Townhall and ScoutTower
	// Thus, they have different structured maps.
	if (m_mapTownhallTexturesHuman.contains(s))
		m_ActiveSeasonTownhallHuman = m_mapTownhallTexturesHuman.at(s);

	if (m_mapTownhallTexturesOrc.contains(s))
		m_ActiveSeasonTownhallOrc = m_mapTownhallTexturesOrc.at(s);

	if (m_mapScoutTowerTexturesHuman.contains(s))
		m_ActiveSeasonScoutTowerTexturesHuman = m_mapScoutTowerTexturesHuman.at(s);

	if (m_mapScoutTowerTexturesOrc.contains(s))
		m_ActiveSeasonScoutTowerTexturesOrc = m_mapScoutTowerTexturesOrc.at(s);
}


void ResourceManager_wartools::switchSeason(Season season) {
	m_currentSeason = season;
	switchSeasonIcons(season);
	switchBuildingSprites(season);
	m_pActiveTileSet = m_mapTileSets[season];
	m_pActiveCritterTexture = m_mapCritters[season];


	// Refresh tile sprites, each Tile object has a pointer
	// to one of the stored sprites
	for (auto& [tile_id, sprite] : m_tileLookUp) {
		int val = m_mapPUDId_to_TileSetIndex[tile_id][season];
		int rowOffset = val / 16;
		int colOffset = val % 16;

		// startoffset
		int iconWidth_px = 32;
		int iconHeight_px = 32;

		Rect rect = { colOffset * iconWidth_px, rowOffset * iconHeight_px, iconWidth_px, iconHeight_px };
		sprite = Sprite(&m_pActiveTileSet, rect);
	}

	m_animDestroyedBuilding.setBaseTexture(m_mapDestroyedSites[BuildingDestroyedType::DestroyedBig][m_currentSeason]);
	m_animDestroyedBuildingSea.setBaseTexture(m_mapDestroyedSites[BuildingDestroyedType::DestroyedBig][m_currentSeason]);

	notifySeasonObservers();
}


void ResourceManager_wartools::switchSeasonIcons(Season s) {
	for (auto const& [color, palette] : mapPlayerPaletteColors) {
		m_ActiveSeasonIcons[color] = m_SeasonIcons[s][color];
	}
}


void ResourceManager_wartools::readIcons() {
	std::string strToFolder("data/graphics/tilesets/");
	SDL_Surface* pSurface_Icons_Summer = IMG_Load((strToFolder + "summer/icons.png").c_str());
	SDL_Surface* pSurface_Icons_Swamp = IMG_Load((strToFolder + "swamp/icons.png").c_str());
	SDL_Surface* pSurface_Icons_Winter = IMG_Load((strToFolder + "winter/icons.png").c_str());
	SDL_Surface* pSurface_Icons_Wasteland = IMG_Load((strToFolder + "wasteland/icons.png").c_str());

	assert(pSurface_Icons_Summer);
	assert(pSurface_Icons_Swamp);
	assert(pSurface_Icons_Winter);
	assert(pSurface_Icons_Wasteland);

	for (auto const& [color, palette] : mapPlayerPaletteColors) {
		SDL_SetPaletteColors(pSurface_Icons_Summer->format->palette, &palette[0], 208, 4);
		SDL_SetPaletteColors(pSurface_Icons_Swamp->format->palette, &palette[0], 208, 4);
		SDL_SetPaletteColors(pSurface_Icons_Winter->format->palette, &palette[0], 208, 4);
		SDL_SetPaletteColors(pSurface_Icons_Wasteland->format->palette, &palette[0], 208, 4);

		m_SeasonIcons[Season::Summer][color] = new Texture(pSurface_Icons_Summer, m_pRenderer, false);
		m_SeasonIcons[Season::Swamp][color] = new Texture(pSurface_Icons_Swamp, m_pRenderer, false);
		m_SeasonIcons[Season::Winter][color] = new Texture(pSurface_Icons_Winter, m_pRenderer, false);
		m_SeasonIcons[Season::Wasteland][color] = new Texture(pSurface_Icons_Wasteland, m_pRenderer, false);
	}

	SDL_FreeSurface(pSurface_Icons_Summer);
	SDL_FreeSurface(pSurface_Icons_Swamp);
	SDL_FreeSurface(pSurface_Icons_Winter);
	SDL_FreeSurface(pSurface_Icons_Wasteland);
}




std::unordered_map<std::type_index, Pud_Icon> mapTypeToIcon = { {typeid(Ballista), Pud_Icon::PUD_ICON_BALLISTA},
																 {typeid(Battleship), Pud_Icon::PUD_ICON_BATTLESHIP},
																 {typeid(Dwarfes), Pud_Icon::PUD_ICON_DWARVES},
																 {typeid(Archer), Pud_Icon::PUD_ICON_ARCHER},
																 {typeid(Destroyer), Pud_Icon::PUD_ICON_ELVEN_DESTROYER},
																 {typeid(Footman), Pud_Icon::PUD_ICON_FOOTMAN},
																 {typeid(GnomishFlyingMaschine), Pud_Icon::PUD_ICON_GNOMISH_FLYING_MACHINE},
																 {typeid(GnomishSubmarine), Pud_Icon::PUD_ICON_GNOMISH_SUBMARINE},
																 {typeid(GryphonRider), Pud_Icon::PUD_ICON_GRYPHON_RIDER},
																 {typeid(Knight), Pud_Icon::PUD_ICON_KNIGHT},
																 {typeid(Mage), Pud_Icon::PUD_ICON_MAGE},
																 {typeid(Tanker), Pud_Icon::PUD_ICON_HUMAN_TANKER},
																 {typeid(Peasant), Pud_Icon::PUD_ICON_PEASANT},
																 {typeid(Transport), Pud_Icon::PUD_ICON_HUMAN_TRANSPORT}
	// Critter missing
};

/*
 * Parameter fraction is only of interest if we, e.g., search for "PUD_ICON_MAGE" which
 * has id 14. All orc units of the same type follow, this means the Death Knight "PUD_ICON_DEATH_KNIGHT"
 * has id 15. Thus, if fraction==orc we simply add 1 to the icon index.
 */
Sprite ResourceManager_wartools::getIcon(Pud_Icon icon, Fraction fraction, PlayerColor color) {
	/*
		PUD_ICON_SWORD_0
	    PUD_ICON_HUMAN_ARR
	    PUD_ICON_HUMAN_SHIELD_0
   */

	if (fraction == Fraction::Alliance)
		return getIcon(icon, color);

	// It is an Orc Icon:    
	// Special cases because we have icons that change during upgrades. Hence, we cant increment as the ordering is, e.g., human sword 0,1,2 and then axe 0,1,2
	if (icon == PUD_ICON_SWORD_0)
		icon = PUD_ICON_AXE_0;
	else if (icon == PUD_ICON_HUMAN_SHIELD_0)
		icon = PUD_ICON_ORC_SHIELD_0;
	else {
		// Unfortunately operator++ is not supported for enums...
		int pos = static_cast<int>(icon) + 1;

		// Special case...
		if (icon == PUD_ICON_HUMAN_GUARD_TOWER || icon == PUD_ICON_HUMAN_CANNON_TOWER) {
			++pos;
		}

		icon = static_cast<Pud_Icon>(pos);
	}

	return getIcon(icon, color);
}


Sprite ResourceManager_wartools::getIcon(Pud_Icon icon, PlayerColor color) {
	int columns = 5; //, rows = 38;
	int iconWidth_px = 46;
	int iconHeight_px = 38;

	int pos = static_cast<int>(icon);
	int nCols = pos % columns; // Get column id
	int nRows = pos / columns; // Get row id

	Rect rect = { nCols * iconWidth_px, nRows * iconHeight_px, iconWidth_px, iconHeight_px };

	if (color == PlayerColor::Neutral) {
		// Neutral Icons do not have a color palette (red, blue, etc.)
		// As it does not matter, simply take the icon texture colored for PlayerColor::Red
		color = PlayerColor::Red;
	}

	return Sprite(&m_ActiveSeasonIcons[color], rect);
}


void ResourceManager_wartools::readCritters() {
	m_mapCritters[Season::Summer] = new Texture("data/graphics/tilesets/summer/neutral/units/critter.png", m_pRenderer);
	m_mapCritters[Season::Swamp] = new Texture("data/graphics/tilesets/swamp/neutral/units/critter.png", m_pRenderer);
	m_mapCritters[Season::Winter] = new Texture("data/graphics/tilesets/winter/neutral/units/critter.png", m_pRenderer);
	m_mapCritters[Season::Wasteland] = new Texture("data/graphics/tilesets/wasteland/neutral/units/critter.png", m_pRenderer);
}


void ResourceManager_wartools::readTileSetTextures() {
	m_mapTileSets[Season::Summer] = new Texture("data/graphics/tilesets/summer/terrain/summer.png", m_pRenderer);
	m_mapTileSets[Season::Swamp] = new Texture("data/graphics/tilesets/swamp/terrain/swamp.png", m_pRenderer);
	m_mapTileSets[Season::Winter] = new Texture("data/graphics/tilesets/winter/terrain/winter.png", m_pRenderer);
	m_mapTileSets[Season::Wasteland] = new Texture("data/graphics/tilesets/wasteland/terrain/wasteland.png", m_pRenderer);
}
