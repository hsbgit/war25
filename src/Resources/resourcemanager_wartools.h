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

#pragma once


#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif


#include <map>
#include <string>
#include <vector>

class Texture;
class Sprite;
class Renderer;



// ToDo: refactoring
#include "Units/Unit.h"


#include <pud.h> // Contains all icon definitions in enum 'Pud_Icon'



class TileSetContainer {
public:
	TileSetContainer(Renderer* pRenderer);

	std::vector<Texture*> vecBuildings;



	Texture* pIcons = nullptr;
	//    BuildingContainer buildingSprites;
	std::map<std::string, Texture*> mapTextures;
};


#include <typeindex>
//enum class Units {Footman, Ballista}
#include "PlayerColor.h"
#include "Animation.h"

class Unit;
class Building;

#include <unordered_map>

#include "Direction.h"


enum class Season { Summer, Winter, Wasteland, Swamp };

/*
 * Contains human and orc textures in all PlayerColors, accessed by their file name.
 * E.g.:
 *      m_mapUnitTextures[dragon.png][PlayerColor::Red]
 */
typedef std::unordered_map<std::type_index, std::unordered_map<PlayerColor, Texture*>> TextureMap;



#include "Buildings/Townhall.h"
typedef std::unordered_map<std::type_index, std::unordered_map<Season, std::unordered_map<PlayerColor, Texture*>>> BuildingTextureMap;
//typedef std::unordered_map<Season, std::unordered_map<Townhall::UpgradeLevel, std::unordered_map<PlayerColor, Texture*>>> MultiBuildingTextureMap;

template<typename UpgradeLevel> using MultiBuildingTextureMap = std::unordered_map<Season, std::unordered_map<UpgradeLevel, std::unordered_map<PlayerColor, Texture*>>>;

template<typename UpgradeLevel> using MultiBuildingActiveSeason = std::unordered_map<UpgradeLevel, std::unordered_map<PlayerColor, Texture*>>;



typedef std::unordered_map<std::type_index, std::unordered_map<Season, Texture*>> NeutralBuildingTextureMap;


enum BuildingDestroyedType { DestroyedSmall, DestroyedBig, DestroyedWater };
typedef std::unordered_map<BuildingDestroyedType, std::unordered_map<Season, Texture*>> BuildingDestroyedMap;


// map  std::type_index zu enum
#include "Sprite.h"

typedef std::unordered_map<Season, std::map<PlayerColor, Texture*>> IconTextureMap;

#include "Global/Fractions.h"

class Object;


// Note: We do not care about memory, each unit/building sprite is stored in all available colors (red, blue, etc.)


/*
 *  Dying:
 *      ballista: explode
 *      dragon: explore
 *      peasant, footman, archer,knight: rotten
 *      Mage: disappear
 */
 // SW, W, NW (mirrored since they are not contained within the sprite set)



class SeasonObserver {
public:
	virtual void notifySeasonChanged(Season newSeason) = 0;
};


#include "Buildings/ScoutTower.h"
#include "Buildings/Townhall.h"


enum CursorTypes { HumanGauntlet, RedEagle, YellowEagle, GreenEagle, OrcishClaw, RedCrosshairs, YellowCrosshairs, GreenCrosshairs, MagnifyingGlass };



class ResourceManager_wartools {

	SDL_Cursor* m_pLastSetCursor = nullptr;
	std::map<CursorTypes, SDL_Cursor*> m_cursors;  // SDL_FreeCursor in dtor

public:
	ResourceManager_wartools(const std::string& pathToGraphicsFolder, Renderer* pRenderer);


	void loadCursors();

	void refreshLoadedCursor();
	void setCursor(CursorTypes type);


	void typeTest(Object* pObj);

	void switchSeason(Season s);
	Season m_currentSeason;


	inline void registerSeasonObserver(SeasonObserver* pObserver) {
		m_vecSeasonObservers.push_back(pObserver);
	}

	inline void deregisterSeasonObserver(SeasonObserver* pObserver) {
		m_vecSeasonObservers.erase(std::remove(m_vecSeasonObservers.begin(), m_vecSeasonObservers.end(), pObserver), m_vecSeasonObservers.end());
	}

	std::vector<SeasonObserver*> m_vecSeasonObservers;

	void notifySeasonObservers() const {
		for (auto& pObserver : m_vecSeasonObservers) {
			pObserver->notifySeasonChanged(m_currentSeason);
		}
	}


	enum class MissileType { Arrow, Axe, Cannon, BallistaBolt, CatapultRock };
	Sprite getMissile(MissileType t);


	/*
	 * As a reference: Each rune has always the same animation state
	 */
	Animation& getRuneAnimation() {
		return m_runeAnimation;
	}

	// Copy, as several animations are created on different timepoints
	Animation getExplosionAnimation() {
		return m_animExplosion;
	}

	// Copy, as several animations are created on different timepoints
	inline Animation getDestroyedBuildingAnimation(bool sea) {
		return sea ? m_animDestroyedBuildingSea : m_animDestroyedBuilding;
	}


	Sprite getBuildingConstructionSprite(int level);
	Texture* m_pTextureConstructionSites = nullptr;
	Sprite m_pSpriteConstructionSiteSmall;
	Sprite m_pSpriteConstructionSiteLarge;



	enum class Ressources { Gold, Wood, Oil, Mana };
	Sprite* getUIElement(Ressources uiElement);
	Sprite getUISprite(const std::string& strPngPath);

	Animation getUnitIdleAnimation(Unit* pUnit, Direction direction);
	Animation getDyingAnimation(Unit* pUnit);


	// Working nur peasant. Walking east, west, north....
	enum class AnimationType { Working_Gold, Working_Lumber, Moving, Attacking, Dying };
	Animation getUnitAnimation(Unit* pUnit, Direction direction, AnimationType animType);
	Animation getBuildingAnimation(Building* pBuilding, AnimationType animType);

	Animation m_animExplosion, m_animDestroyedBuilding, m_animDestroyedBuildingSea;

	void createUnitAnimations(std::type_index correspondingType, int iconSquareWidth_px, int spriteCountMoving, int spriteCountAttacking, int spriteCountDying);
	void createVesselAnimations(std::type_index correspondingType, int iconSquareWidth_px);



	Sprite getCritterSprite(Direction direction, bool dead = false);
	void readCritters();
	Texture* m_pActiveCritterTexture = nullptr;
	std::unordered_map<Season, Texture*> m_mapCritters;


	// Sprite getIcon(const Unit* pUnit);
	Sprite getIcon(Pud_Icon icon, Fraction fraction, PlayerColor color);
	Sprite getIcon(Pud_Icon icon, PlayerColor color = PlayerColor::Neutral);


	TextureMap m_mapUnitTexturesHuman, m_mapUnitTexturesOrc, m_mapUnitTexturesNeutral;

	//  Animation m_idleMage, m_idleBattleShip, m_idleGryphonRider;

	std::unordered_map<std::type_index, std::unordered_map<Direction, Animation>> mapIdleAnimations;
	std::unordered_map<std::type_index, std::unordered_map<Direction, Animation>> mapUnitMovementAnimations, mapUnitAttackAnimations, mapUnitDyingAnimations;



	const Sprite* getBuildingIcon(int i) const;
	Sprite* pFarm = nullptr;

	std::vector<Sprite*> vecSprites;


	Texture* pTileset = nullptr;


	std::map<BuffType, Sprite*> m_mapBuffSprites;

private:
	void createUnitSprites(Fraction fraction);
	void createidleAnimations();
	void createidleAnimation(std::type_index correspondingType, int squareWidth_px, int rows, int cols, unsigned int interval_ms, Point posOffset);

	std::vector<Sprite*> extractSprites(Texture** pTexture, int cntRows, int cntCols, int iconWidth_px, int iconHeight_px);

	static const std::string strPrefix;

	void readDirectoryRecursively(const std::string& pathToGraphicsFolder);



	// Icons -----------------------------------------------------
public: // ToDo: Debug
	/*
	 * All these m_ActiveSeason<...> variables are actually just for fun.
	 * They allow easy tileset switching during runtime;
	 *
	 * Each sprite has a Texture**, which points to a Texture* stored in one
	 * of the m_ActiveSeason<...> variables. Tileset switching simply means
	 * we set a new value for the m_ActiveSeason<...> variables and that's it.
	 * All sprites stay valid!
	 */
	std::map<PlayerColor, Texture*> m_ActiveSeasonIcons; // Contains the currently valid icon sets (Summer, Winter, Wasteland)




private:
	void createRuneAnimation();
	Animation m_runeAnimation;


	void createBuffIcons();
	Texture* pBuffIcons = nullptr;


	void createExplosionAnimation();
	void createDestroyedSiteAnimations();
	void loadAndStoreDestroyedBuildingTexture(const std::string& basePath, BuildingDestroyedType type, Season season);

	IconTextureMap m_SeasonIcons; // Stores all icon textures (Summer, Winter, Wasteland) in all colors
	void switchSeasonIcons(Season s);


	void readIcons();
	void readMissiles();
	std::unordered_map<MissileType, Texture*> m_mapMissiles;

	// Icons -----------------------------------------------------



	// Buildings -----------------------------------------------------
	void readBuildings(bool orcs);
	void readBuildingSprite(Season season, const std::string& pathToFile, bool orcs);

	BuildingTextureMap m_mapBuildingTexturesHuman, m_mapBuildingTexturesOrc;
	std::unordered_map<std::type_index, std::unordered_map<PlayerColor, Texture*>> m_ActiveSeasonBuildingsHuman, m_ActiveSeasonBuildingsOrc;

	MultiBuildingTextureMap<Townhall::UpgradeLevel> m_mapTownhallTexturesHuman, m_mapTownhallTexturesOrc;
	MultiBuildingActiveSeason<Townhall::UpgradeLevel> m_ActiveSeasonTownhallHuman, m_ActiveSeasonTownhallOrc;

	MultiBuildingTextureMap<ScoutTower::UpgradeLevel> m_mapScoutTowerTexturesHuman, m_mapScoutTowerTexturesOrc;
	MultiBuildingActiveSeason<ScoutTower::UpgradeLevel> m_ActiveSeasonScoutTowerTexturesHuman, m_ActiveSeasonScoutTowerTexturesOrc;

public: // Debug,w ar private
	NeutralBuildingTextureMap m_mapNeutralBuildingTextures, m_mapActiveNeutralBuildingTextures;
private:
	void switchBuildingSprites(Season season);
public:
	Sprite getBuildingSprite(Building* pBuilding, Fraction fraction, PlayerColor color, bool underConstruction = false);
	Sprite getNeutralBuildingSprite(Building* pBuilding, int index = 0);
private:

	// Buildings -----------------------------------------------------

	BuildingDestroyedMap m_mapDestroyedSites;


	// TileSets --------------------------------------------------
public: // debug
	Texture* m_pActiveTileSet = nullptr;

	Sprite* getTileSprite(int tile_id);

	// Some special tiles are not part of a pud file (or better: they can not as they can not be placed inside an editor)
	// E.g., chopped wood, destroyed rocks, ...
	// Still unsure about these things, they are from top of the summer.lua file
	enum SpecialTile { TopOneTree = 0x8000, MidOneTree, BotOneTree, RemovedWood, TopOnRock, MidOneRock, BotOneRock, RemovedRock, DestroyedWalls };

private:
	void parseLUATileSetFile(Season season, const std::string& strFile);
	std::map<int, std::unordered_map<Season, int>> m_mapPUDId_to_TileSetIndex;      // unordered nehmen
	// This map prevents creating sprites several times
	// problem bei "wood"?
	std::unordered_map<int, Sprite> m_tileLookUp;



	std::unordered_map<Season, Texture*> m_mapTileSets;
	void readTileSetTextures();
	// TileSets --------------------------------------------------


	void loadTilesets();
	Renderer* m_pRenderer = nullptr;
	//public:
		//TileSetContainer tileSetContainer;
};


extern ResourceManager_wartools* g_pRessourceManager;

