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


#include "resourcemanager_wartools.h"
#include "ResourceMappings.h"
#include "Texture.h"
#include "Sprite.h"
#include "Renderer.h"
#include "PlayerColor.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

// Constants for resource loading
namespace {
    constexpr int PALETTE_COLOR_OFFSET = 208;
    constexpr int PALETTE_COLOR_COUNT = 4;
    constexpr int EXPLOSION_SPRITE_SIZE = 320 / 5;
    constexpr int EXPLOSION_SPRITE_COUNT = 16;
    constexpr int BUBBLE_WIDTH_PX = 504 / 7;
    constexpr unsigned int DEFAULT_UNIT_IDLE_INTERVAL = 10000;
    constexpr unsigned int VESSEL_ANIMATION_INTERVAL = 1500;
    constexpr unsigned int EXPLOSION_INTERVAL = 100;
    constexpr unsigned int RUNE_ANIMATION_INTERVAL = 150;
}

ResourceManager_wartools* g_pRessourceManager = nullptr;


const std::string ResourceManager_wartools::strPrefix = std::string("data") + PATH_SEPARATOR + std::string("graphics") + PATH_SEPARATOR;

ResourceManager_wartools::~ResourceManager_wartools() {
	for (auto& [type, cursor] : m_cursors) {
		(void)type;
		SDL_FreeCursor(cursor);
	}
}

void ResourceManager_wartools::notifySeasonObservers() const {
	// Work on a copy: an observer may deregister itself while handling the
	// notification (for example when a map is replaced).
	const auto observers = m_vecSeasonObservers;
	for (SeasonObserver* observer : observers)
		if (observer) observer->notifySeasonChanged(m_currentSeason);
}

const Sprite* ResourceManager_wartools::getBuffIcon(BuffType type) const {
	const auto found = m_buffSprites.find(type);
	return found == m_buffSprites.end() ? nullptr : found->second.get();
}


// Unfortunately required as SDL_SetCursor does not always work properly -> Windows default cursor is set again so we call refreshLoadedCursor
#include "Animation.h"
#include "Units/Unit.h"
#include "Player.h"
#include "Global/GlobalAttributes.h"
#include "Units/Critter.h"
#include "Buildings/Building.h"

// Unit includes
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

// Building includes
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
#include "Global/BuildingPlacement.h"

void ResourceManager_wartools::createUnitAnimations(std::type_index correspondingType, int iconSquareWidth_px, int spriteCountMoving, int spriteCountAttacking, int spriteCountDying) {
	TextureMap& textMap = m_mapUnitTexturesHuman; // Does not matter whether orc or human, will be changed individually, e.g., in getUnitIdleAnimation
	PlayerColor color = PlayerColor::Red; // Color does not matter, will be changed individually, e.g., in getUnitIdleAnimation

	Texture* pTexture = textMap[correspondingType][color];
	assert(pTexture);


	for (Direction direction : AllDirections) {
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
		if (spriteCountMoving > 0) {
			Animation idle(textMap[correspondingType][color], interval_ms);
			const Rect standingFrame = {colOffset * iconSquareWidth_px, 0,
				iconSquareWidth_px, iconSquareWidth_px};
			idle.addSprite(Sprite(idle.getBaseTexture(), standingFrame, posOffset, flip));
			mapIdleAnimations[correspondingType][direction] = idle;
		}



		// Attack animations
		startOffset += spriteCountMoving;
		Animation animAttacking(textMap[correspondingType][color], interval_ms);
		for (int rowOffset = startOffset; rowOffset < startOffset + spriteCountAttacking; ++rowOffset) {
			Rect r = { colOffset * iconSquareWidth_px, rowOffset * iconSquareWidth_px, iconSquareWidth_px, iconSquareWidth_px };
			animAttacking.addSprite(Sprite(animAttacking.getBaseTexture(), r, posOffset, flip));
		}
		if (spriteCountAttacking > 0)
			mapUnitAttackAnimations[correspondingType][direction] = animAttacking;


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
	for (Direction direction : AllDirections) {
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
		unsigned int interval_ms = VESSEL_ANIMATION_INTERVAL;
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
		int bubbleWidth_px = BUBBLE_WIDTH_PX;
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

	ResourceType carriedResource = ResourceType::Nothing;
	if (auto* pCollector = dynamic_cast<IResourceCollector*>(pUnit))
		carriedResource = pCollector->currentlyCarrying();

	// Working animations always use the unloaded worker sheet. The resource is
	// only assigned after the work completes, at which point movement switches
	// to the corresponding carried-resource sheet.
	if (animType == AnimationType::Working_Gold || animType == AnimationType::Working_Lumber)
		carriedResource = ResourceType::Nothing;

	Texture* pCorrectColorTexture = getUnitTexture(pUnit, carriedResource);
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

	if (animType == AnimationType::Attacking) {
		// Several vessel sheets contain directional idle/movement frames but no
		// dedicated attack rows. Combat must still work for those units.
		if (!mapUnitAttackAnimations.contains(typeid(*pUnit)) ||
			!mapUnitAttackAnimations[typeid(*pUnit)].contains(direction))
			return getUnitIdleAnimation(pUnit, direction);
		Animation a(mapUnitAttackAnimations[typeid(*pUnit)][direction]);
		a.setBaseTexture(pCorrectColorTexture);
		return a;
	}

	if (animType == AnimationType::Working_Gold ||
		animType == AnimationType::Working_Lumber) {
		assert(mapUnitAttackAnimations.contains(typeid(*pUnit)));
		assert(mapUnitAttackAnimations[typeid(*pUnit)].contains(direction));
		Animation a(mapUnitAttackAnimations[typeid(*pUnit)][direction]);
		a.setBaseTexture(pCorrectColorTexture);
		return a;
	}

	throw std::logic_error("AnimationType not yet implemented!");
}


Texture* ResourceManager_wartools::getUnitTexture(Unit* pUnit, ResourceType carriedResource) {
	if (typeid(*pUnit) == typeid(Skeleton))
		return m_mapUnitTexturesNeutral[typeid(Skeleton)][PlayerColor::Red];

	TextureMap& baseTextures = (pUnit->getFraction() == Fraction::Orc)
		? m_mapUnitTexturesOrc : m_mapUnitTexturesHuman;
	const std::type_index type = typeid(*pUnit);
	const PlayerColor color = pUnit->getOwner()->getColor();

	if ((type == typeid(Peasant) || type == typeid(Tanker)) &&
		carriedResource != ResourceType::Nothing) {
		auto& resourceTextures = (pUnit->getFraction() == Fraction::Orc)
			? m_mapWorkerResourceTexturesOrc : m_mapWorkerResourceTexturesHuman;
		if (resourceTextures.contains(carriedResource) &&
			resourceTextures.at(carriedResource).contains(type) &&
			resourceTextures.at(carriedResource).at(type).contains(color))
			return resourceTextures.at(carriedResource).at(type).at(color);
	}

	if (!baseTextures.contains(type) || !baseTextures.at(type).contains(color))
		return nullptr;
	return baseTextures.at(type).at(color);
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
	const std::type_index type = typeid(*pUnit);
	auto* animations = (pUnit->isSeaObject() || pUnit->isAirObject())
		? &mapUnitMovementAnimations : &mapIdleAnimations;
	if (!animations->contains(type))
		return Animation();
	auto& directional = animations->at(type);
	if (!directional.contains(direction)) {
		// Some exceptional sprites (notably skeletons) have only a north idle.
		// Falling back is harmless and must not flood stderr every few seconds.
		if (!directional.contains(Direction::N))
			return Animation();
		direction = Direction::N;
	}
	Animation a(directional.at(direction));

	ResourceType carriedResource = ResourceType::Nothing;
	if (auto* pCollector = dynamic_cast<IResourceCollector*>(pUnit))
		carriedResource = pCollector->currentlyCarrying();
	Texture* pCorrectColorTexture = getUnitTexture(pUnit, carriedResource);
	if (!pCorrectColorTexture) {
		std::cerr << "ERROR: Unit texture not found!" << std::endl;
		return a; // Return animation without texture
	}

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
			anim.addSprite(Sprite(anim.getBaseTexture(), rect, posOffset));
			rect.y += squareWidth_px;
		}

		rect.x += squareWidth_px;
	}

	mapIdleAnimations[correspondingType][Direction::N] = anim;
}


void ResourceManager_wartools::createExplosionAnimation() {
	int iconSquareWidth_px = EXPLOSION_SPRITE_SIZE;
	int spriteCnt = EXPLOSION_SPRITE_COUNT;

	Texture* pTexture = makeTexture("data/graphics/missiles/explosion.png", m_pRenderer);

	Point posOffset = { -16, -16 };

	int interval_ms = EXPLOSION_INTERVAL;
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

	m_mapDestroyedSites[type][season] = makeTexture(pSurface, m_pRenderer, false);
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

	Texture* pTexRune = makeTexture(pSurface, m_pRenderer, false);
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

	pBuffIcons = makeTexture(pSurface, m_pRenderer, false);
	SDL_free(pSurface);

	const int squareWidth_px = 16;

	Rect rect = { 0, 0, squareWidth_px, squareWidth_px };
	m_buffSprites[BuffType::Bloodlust] = std::make_unique<Sprite>(&pBuffIcons, rect);

	rect.y += 16;
	m_buffSprites[BuffType::Haste] = std::make_unique<Sprite>(&pBuffIcons, rect);


	rect.y += 16;
	m_buffSprites[BuffType::Slow] = std::make_unique<Sprite>(&pBuffIcons, rect);

	rect.y += 16;
	m_buffSprites[BuffType::Invisibility] = std::make_unique<Sprite>(&pBuffIcons, rect);


	rect.y += 16;
	m_buffSprites[BuffType::UnholyArmor] = std::make_unique<Sprite>(&pBuffIcons, rect);
}


void ResourceManager_wartools::createidleAnimations() {
	// See png files for row/column structure. Skeletons use their movement
	// sheet for idle frames and are initialized separately below.
	createidleAnimation(typeid(Ballista), 320 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(Dwarfes), 280 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(Archer), 360 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(Footman), 360 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(GnomishFlyingMaschine), 400 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(GryphonRider), 400 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(Knight), 360 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(Mage), 360 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);
	createidleAnimation(typeid(Peasant), 360 / 5, 1, 1, DEFAULT_UNIT_IDLE_INTERVAL);




	createVesselAnimations(typeid(Battleship), 440 / 5);
	createVesselAnimations(typeid(Destroyer), 400 / 5);
	createVesselAnimations(typeid(GnomishSubmarine), 360 / 5);
	createVesselAnimations(typeid(Tanker), 360 / 5);
	createVesselAnimations(typeid(Transport), 360 / 5);


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

	// Skeletons use five frames horizontally and a living/death row pair per
	// direction, unlike the regular unit sheets.
	Texture* skeletonTexture = m_mapUnitTexturesNeutral[typeid(Skeleton)][PlayerColor::Red];
	assert(skeletonTexture);
	for (int directionValue = 0; directionValue < 8; ++directionValue) {
		Direction direction = static_cast<Direction>(directionValue);
		int sourceDirection = directionValue;
		bool flipped = false;
		if (sourceDirection > 4) {
			sourceDirection -= (sourceDirection - 4) * 2;
			flipped = true;
		}
		constexpr int frameSize = 56;
		const Point offset{-12, -12};
		Animation movement(skeletonTexture, 120);
		Animation attack(skeletonTexture, 120);
		Animation dying(skeletonTexture, 160, true);
		for (int frame = 0; frame < 5; ++frame) {
			const Rect living{frame * frameSize, sourceDirection * 2 * frameSize,
				frameSize, frameSize};
			movement.addSprite(Sprite(movement.getBaseTexture(), living, offset, flipped));
			attack.addSprite(Sprite(attack.getBaseTexture(), living, offset, flipped));
			const Rect dead{frame * frameSize, (sourceDirection * 2 + 1) * frameSize,
				frameSize, frameSize};
			dying.addSprite(Sprite(dying.getBaseTexture(), dead, offset, flipped));
		}
		mapUnitMovementAnimations[typeid(Skeleton)][direction] = movement;
		mapUnitAttackAnimations[typeid(Skeleton)][direction] = attack;
		mapUnitDyingAnimations[typeid(Skeleton)][direction] = dying;
		mapIdleAnimations[typeid(Skeleton)][direction] = movement.singleFrame(0);
	}


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


								// Individual sprite pointers follow the active texture indirection.
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
		if (texMap.contains(type) && texMap.at(type).contains(color))
			pTexture = &texMap.at(type).at(color);
	}

	if (!pTexture || !*pTexture) {
		std::cerr << "Missing building texture for type " << type.name()
			<< ", faction " << static_cast<int>(fraction)
			<< ", color " << static_cast<int>(color)
			<< "; using placeholder sprite" << std::endl;
		return getUISprite("orc//cursors//red_crosshairs.png");
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

Sprite ResourceManager_wartools::getBuildingPlacementSprite(PeasantActions action, Fraction fraction,
	PlayerColor color) {
	Texture** texture = nullptr;
	if (action == PeasantActions::BuildScoutTower) {
		auto& towers = fraction == Fraction::Orc
			? m_ActiveSeasonScoutTowerTexturesOrc : m_ActiveSeasonScoutTowerTexturesHuman;
		if (towers.contains(ScoutTower::UpgradeLevel::ScoutTower) &&
			towers[ScoutTower::UpgradeLevel::ScoutTower].contains(color))
			texture = &towers[ScoutTower::UpgradeLevel::ScoutTower][color];
	}
	else if (action == PeasantActions::BuildTownhall) {
		auto& halls = fraction == Fraction::Orc
			? m_ActiveSeasonTownhallOrc : m_ActiveSeasonTownhallHuman;
		if (halls.contains(Townhall::UpgradeLevel::Townhall) &&
			halls[Townhall::UpgradeLevel::Townhall].contains(color))
			texture = &halls[Townhall::UpgradeLevel::Townhall][color];
	}
	else {
		auto& buildings = fraction == Fraction::Orc
			? m_ActiveSeasonBuildingsOrc : m_ActiveSeasonBuildingsHuman;
		const std::type_index type = buildingTypeForAction(action);
		if (buildings.contains(type) && buildings[type].contains(color))
			texture = &buildings[type][color];
	}
	if (!texture || !*texture)
		return getUISprite("orc//cursors//red_crosshairs.png");
	int width = 0;
	int height = 0;
	SDL_QueryTexture((*texture)->getSDLTexture(), nullptr, nullptr, &width, &height);
	return Sprite(texture, {0, 0, width, width});
}




std::unordered_map<Season, std::string> mapSeasonToPath = {
	{Season::Summer, "summer/"},
	{Season::Swamp, "swamp/"},
	{Season::Wasteland, "wasteland/"},
	{Season::Winter, "winter/"}
};


void ResourceManager_wartools::readBuildings() {
	std::string strToFolder("data/graphics/tilesets/");

	for (Season season : AllSeasons) {
		std::string strPathToSeasonFolder(strToFolder);
		strPathToSeasonFolder += mapSeasonToPath[season];

		for (const auto& [key_imgFileName, value] : ResourceMappings::getHumanBuildingMap()) {
			std::string tmp = strPathToSeasonFolder + "human/buildings/" + key_imgFileName;
			readBuildingSprite(season, tmp, false);
		}

		for (const auto& [key_imgFileName, value] : ResourceMappings::getOrcBuildingMap()) {
			std::string tmp = strPathToSeasonFolder + "orc/buildings/" + key_imgFileName;
			readBuildingSprite(season, tmp, true);
		}

		for (const auto& [key_imgFileName, value] : ResourceMappings::getNeutralBuildingMap()) {
			std::string tmp = strPathToSeasonFolder + "neutral/buildings/" + key_imgFileName;

			SDL_Surface* pSurface = IMG_Load(tmp.c_str());
			if (!pSurface)
				continue; // return

			const std::string pngPath = fs::path(tmp).filename().string();

			std::type_index correspondingType = ResourceMappings::getNeutralBuildingMap().at(pngPath);
			m_mapNeutralBuildingTextures[correspondingType][season] = makeTexture(pSurface, m_pRenderer, false);
			SDL_FreeSurface(pSurface);
		}
	}
}


void ResourceManager_wartools::readBuildingSprite(Season season, const std::string& pathToFile, bool orcs) {
	SDL_Surface* pSurface = IMG_Load(pathToFile.c_str());
	if (!pSurface)
		return;

	assert(pSurface);

	const std::string pngPath = fs::path(pathToFile).filename().string();

	BuildingTextureMap& texTargetMap = orcs ? m_mapBuildingTexturesOrc : m_mapBuildingTexturesHuman;

	const MapFileNameToTypeID& mapFileNameToTypeID = orcs ? ResourceMappings::getOrcBuildingMap() : ResourceMappings::getHumanBuildingMap();


	for (auto const& [color, palette] : mapPlayerPaletteColors) {
		SDL_SetPaletteColors(pSurface->format->palette, &palette[0], PALETTE_COLOR_OFFSET, PALETTE_COLOR_COUNT);

		std::type_index correspondingType = mapFileNameToTypeID.at(pngPath);
		Texture* pTexture = makeTexture(pSurface, m_pRenderer, false);

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
			const std::string pngPath = entry.path().filename().string();
			SDL_Surface* pSurface = IMG_Load(entry.path().string().c_str());
			assert(pSurface);

			TextureMap* targetMap = textMap;
			// Eye of Kilrogg has no implementation yet and historically shared
			// Ballista's type id, which made filesystem iteration replace the
			// actual Orc catapult texture with the flying eye.
			if (pngPath == "eye_of_kilrogg.png") {
				SDL_FreeSurface(pSurface);
				continue;
			}
			if (fraction == Fraction::Alliance && pngPath == "peasant_with_gold.png")
				targetMap = &m_mapWorkerResourceTexturesHuman[ResourceType::Gold];
			else if (fraction == Fraction::Alliance && pngPath == "peasant_with_wood.png")
				targetMap = &m_mapWorkerResourceTexturesHuman[ResourceType::Wood];
			else if (fraction == Fraction::Orc && pngPath == "peon_with_gold.png")
				targetMap = &m_mapWorkerResourceTexturesOrc[ResourceType::Gold];
			else if (fraction == Fraction::Orc && pngPath == "peon_with_wood.png")
				targetMap = &m_mapWorkerResourceTexturesOrc[ResourceType::Wood];
			else if (fraction == Fraction::Alliance && pngPath == "oil_tanker_full.png")
				targetMap = &m_mapWorkerResourceTexturesHuman[ResourceType::Oil];
			else if (fraction == Fraction::Orc && pngPath == "oil_tanker_full.png")
				targetMap = &m_mapWorkerResourceTexturesOrc[ResourceType::Oil];

			for (auto const& [color, palette] : mapPlayerPaletteColors) {
				SDL_SetPaletteColors(pSurface->format->palette, &palette[0], 208, 4);

				std::type_index correspondingType = ResourceMappings::getUnitTypeMap().at(pngPath);
				(*targetMap)[correspondingType][color] = makeTexture(pSurface, m_pRenderer, false);
			}
			SDL_FreeSurface(pSurface);
		}
	}
}


ResourceManager_wartools::ResourceManager_wartools(const std::string& pathToGraphicsFolder, Renderer* pRenderer) : m_pRenderer(pRenderer) {
	if (!m_pRenderer)
		throw std::invalid_argument("ResourceManager requires a renderer");
	if (!fs::is_directory(pathToGraphicsFolder))
		throw std::runtime_error("Graphics directory does not exist: " + pathToGraphicsFolder);


	loadCursors();


	//readDirectoryRecursively(pathToGraphicsFolder + "/ui");
	readIcons();
	readTileSetTextures();
	readCritters();
	readBuildings();
	loadCoastalConstructionSprites();
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

		m_pTextureConstructionSites = makeTexture(pSurface, m_pRenderer);
		Rect size = { 0, 0, 63, 63 };
		m_pSpriteConstructionSiteSmall = Sprite(&m_pTextureConstructionSites, size);

		size.y = 64;
		m_pSpriteConstructionSiteLarge = Sprite(&m_pTextureConstructionSites, size);
	}


	switchSeason(Season::Summer);
}


void ResourceManager_wartools::readMissiles() {
	std::string strToFolder("data/graphics/missiles/");
	m_pCorpseTexture = makeTexture(
		"data/graphics/neutral/units/corpses.png", m_pRenderer);

	m_mapMissiles[MissileType::Arrow] = makeTexture(strToFolder + "arrow.png", m_pRenderer);
	m_mapMissiles[MissileType::Axe] = makeTexture(strToFolder + "axe.png", m_pRenderer);
	m_mapMissiles[MissileType::BallistaBolt] = makeTexture(strToFolder + "ballista_bolt.png", m_pRenderer);
	m_mapMissiles[MissileType::Cannon] = makeTexture(strToFolder + "cannon.png", m_pRenderer);
	m_mapMissiles[MissileType::CatapultRock] = makeTexture(strToFolder + "catapult_rock.png", m_pRenderer);
	m_mapMissiles[MissileType::Fireball] = makeTexture(strToFolder + "fireball.png", m_pRenderer);
	m_mapMissiles[MissileType::DragonBreath] = makeTexture(strToFolder + "dragon_breath.png", m_pRenderer);
	m_mapMissiles[MissileType::GryphonHammer] = makeTexture(strToFolder + "gryphon_hammer.png", m_pRenderer);
	m_mapMissiles[MissileType::SubmarineMissile] = makeTexture(strToFolder + "submarine_missile.png", m_pRenderer);
	m_mapMissiles[MissileType::TurtleMissile] = makeTexture(strToFolder + "turtle_missile.png", m_pRenderer);
	m_mapMissiles[MissileType::NormalSpell] = makeTexture(strToFolder + "normal_spell.png", m_pRenderer);
	m_mapMissiles[MissileType::TouchOfDeath] = makeTexture(strToFolder + "touch_of_death.png", m_pRenderer);

	auto loadGridAnimation = [this, &strToFolder](SpellVisual visual,
		const std::string& file, int frameSize, int interval) {
		Texture* texture = makeTexture(strToFolder + file, m_pRenderer);
		Animation animation(texture, interval);
		for (int y = 0; y + frameSize <= texture->getHeight(); y += frameSize)
			for (int x = 0; x + frameSize <= texture->getWidth(); x += frameSize)
				animation.addSprite(Sprite(animation.getBaseTexture(),
					{x, y, frameSize, frameSize}));
		m_spellAnimations[visual] = animation;
	};
	loadGridAnimation(SpellVisual::FlameShield, "flame_shield.png", 32, 100);
	loadGridAnimation(SpellVisual::Blizzard, "blizzard.png", 32, 120);
	loadGridAnimation(SpellVisual::Whirlwind, "tornado.png", 56, 120);
	loadGridAnimation(SpellVisual::DeathAndDecay, "death_and_decay.png", 32, 120);
	m_pAirUnitShadowTexture = makeTexture(strToFolder + "unit_shadow.png", m_pRenderer);
	m_airUnitShadow = Sprite(&m_pAirUnitShadowTexture, {0, 64, 32, 32});

	m_pSmallFireTexture = makeTexture(strToFolder + "small_fire.png", m_pRenderer);
	m_smallFireAnimation = Animation(m_pSmallFireTexture, 100);
	// The looping flame consists of the five equally anchored frames in the
	// first row. The isolated, much shorter image in row two is not part of
	// this loop; including it made the flame visibly blink once per cycle.
	for (int x = 0; x < 5 * 32; x += 32)
		m_smallFireAnimation.addSprite(Sprite(
			m_smallFireAnimation.getBaseTexture(), {x, 0, 32, 32}));

	m_pBigFireTexture = makeTexture(strToFolder + "big_fire.png", m_pRenderer);
	m_bigFireAnimation = Animation(m_pBigFireTexture, 100);
	for (int y = 0; y < m_pBigFireTexture->getHeight(); y += 48)
		for (int x = 0; x < m_pBigFireTexture->getWidth(); x += 48)
			m_bigFireAnimation.addSprite(Sprite(
				m_bigFireAnimation.getBaseTexture(), {x, y, 48, 48}));
}

Animation ResourceManager_wartools::getSpellAnimation(SpellVisual visual) const {
	return m_spellAnimations.at(visual);
}


Animation ResourceManager_wartools::getBuildingFireAnimation(bool large) const {
	return large ? m_bigFireAnimation : m_smallFireAnimation;
}

Animation ResourceManager_wartools::getCorpseAnimation(
	const Unit* unit, Direction direction) const {
	assert(unit && m_pCorpseTexture);
	Animation corpse(m_pCorpseTexture, 500, true);
	int column = static_cast<int>(direction);
	bool flipped = false;
	if (column > 4) {
		column -= (column - 4) * 2;
		flipped = true;
	}
	constexpr int frameSize = 72;
	const int footprintPixels = gMapUnitTilesize.at(typeid(*unit)) * Tile::TileWidth_px;
	const Point offset{(footprintPixels - frameSize) / 2,
		(footprintPixels - frameSize) / 2};
	auto appendRepeated = [&](int row, int repetitions) {
		for (int i = 0; i < repetitions; ++i)
			corpse.addSprite(Sprite(corpse.getBaseTexture(),
				{column * frameSize, row * frameSize, frameSize, frameSize},
				offset, flipped));
	};
	// Faction-specific body, exposed skeleton, then scattered bones.
	appendRepeated(unit->getFraction() == Fraction::Alliance ? 2 : 1, 7);
	appendRepeated(4, 7);
	appendRepeated(5, 5);
	return corpse;
}


Sprite ResourceManager_wartools::getMissile(MissileType t, Direction direction) {
	assert(m_mapMissiles.contains(t));
	Texture* texture = m_mapMissiles.at(t);
	const int frameWidth = texture->getWidth() / 5;
	const int frameHeight = std::min(frameWidth, texture->getHeight());
	int frame = 0;
	bool flipped = false;
	switch (direction) {
	case Direction::N:  frame = 0; break;
	case Direction::NE: frame = 1; break;
	case Direction::E:  frame = 2; break;
	case Direction::SE: frame = 3; break;
	case Direction::S:  frame = 4; break;
	case Direction::SW: frame = 3; flipped = true; break;
	case Direction::W:  frame = 2; flipped = true; break;
	case Direction::NW: frame = 1; flipped = true; break;
	}
	return Sprite(&m_mapMissiles.at(t), {frame * frameWidth, 0, frameWidth, frameHeight}, flipped);
}


Sprite ResourceManager_wartools::getBuildingConstructionSprite(int level) {
	assert(level == 1 || level == 2);
	assert(m_pTextureConstructionSites);

	return (level == 1) ? m_pSpriteConstructionSiteSmall : m_pSpriteConstructionSiteLarge;
}


Sprite ResourceManager_wartools::getBuildingConstructionSprite(Building* pBuilding, int level) {
	assert(pBuilding);
	assert(level == 1 || level == 2);
	const std::type_index type = typeid(*pBuilding);
	const Fraction fraction = pBuilding->getFraction();
	if (!m_coastalConstructionTextures.contains(type) ||
		!m_coastalConstructionTextures.at(type).contains(fraction))
		return getBuildingConstructionSprite(level);

	Texture*& texture = m_coastalConstructionTextures.at(type).at(fraction);
	const int side = texture->getWidth();
	return Sprite(&texture, {0, (level - 1) * side, side, side});
}


void ResourceManager_wartools::loadCoastalConstructionSprites() {
	const std::pair<std::type_index, std::string> buildings[] = {
		{typeid(Shipyard), "shipyard_construction_site.png"},
		{typeid(Foundry), "foundry_construction_site.png"},
		{typeid(OilRefinery), "refinery_construction_site.png"}
	};
	for (const auto& [type, file] : buildings) {
		m_coastalConstructionTextures[type][Fraction::Alliance] = makeTexture(
			"data/graphics/human/buildings/" + file, m_pRenderer);
		m_coastalConstructionTextures[type][Fraction::Orc] = makeTexture(
			"data/graphics/orc/buildings/" + file, m_pRenderer);
	}
}


Sprite ResourceManager_wartools::getUISprite(const std::string& strPngPath) {
	const std::string strPathUIFolder("data/graphics/ui/");
	std::string strPathToElement = strPathUIFolder + strPngPath;

	if (!m_uiTextures.contains(strPathToElement)) {
		m_uiTextures[strPathToElement] = makeTexture(strPathToElement, m_pRenderer);
	}

	return Sprite(&m_uiTextures[strPathToElement]);
}
