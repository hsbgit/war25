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


#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif


#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>

class Texture;
class Sprite;
class Renderer;



#include "Units/UnitActions.h"
#include "Resources/Season.h"


#include <pud.h> // Contains all icon definitions in enum 'Pud_Icon'



#include <typeindex>
//enum class Units {Footman, Ballista}
#include "PlayerColor.h"
#include "Animation.h"

class Unit;
class Building;

#include <unordered_map>

#include "Direction.h"
#include "Units/Resource/ResourceType.h"


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


enum class BuildingDestroyedType : uint8_t {
	DestroyedSmall = 0,
	DestroyedBig = 1,
	DestroyedWater = 2
};
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



#include "Buildings/ScoutTower.h"
#include "Buildings/Townhall.h"


enum class CursorTypes : uint8_t {
	HumanGauntlet = 0,
	RedEagle = 1,
	YellowEagle = 2,
	GreenEagle = 3,
	OrcishClaw = 4,
	RedCrosshairs = 5,
	YellowCrosshairs = 6,
	GreenCrosshairs = 7,
	MagnifyingGlass = 8
};



class ResourceManager_wartools {

	SDL_Cursor* m_pLastSetCursor = nullptr;
	std::map<CursorTypes, SDL_Cursor*> m_cursors;  // SDL_FreeCursor in dtor

public:
	ResourceManager_wartools(const std::string& pathToGraphicsFolder, Renderer* pRenderer);
	~ResourceManager_wartools();
	ResourceManager_wartools(const ResourceManager_wartools&) = delete;
	ResourceManager_wartools& operator=(const ResourceManager_wartools&) = delete;


	void loadCursors();

	void refreshLoadedCursor();
	void setCursor(CursorTypes type);


	void switchSeason(Season s);
	Season currentSeason() const { return m_currentSeason; }


	inline void registerSeasonObserver(SeasonObserver* pObserver) {
		if (pObserver && std::find(m_vecSeasonObservers.begin(), m_vecSeasonObservers.end(), pObserver) == m_vecSeasonObservers.end())
			m_vecSeasonObservers.push_back(pObserver);
	}

	inline void deregisterSeasonObserver(SeasonObserver* pObserver) {
		m_vecSeasonObservers.erase(std::remove(m_vecSeasonObservers.begin(), m_vecSeasonObservers.end(), pObserver), m_vecSeasonObservers.end());
	}


	enum class MissileType { Arrow, Axe, Cannon, BigCannon, BallistaBolt, CatapultRock,
		Fireball, DragonBreath, GryphonHammer, SubmarineMissile, TurtleMissile,
		NormalSpell, TouchOfDeath };
	Sprite getMissile(MissileType t, Direction direction = Direction::N,
		int animationStage = 0);
	enum class SpellVisual { FlameShield, Blizzard, Whirlwind, DeathAndDecay };
	Animation getSpellAnimation(SpellVisual visual) const;
	const Sprite& getAirUnitShadow() const { return m_airUnitShadow; }


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
	Sprite getBuildingConstructionSprite(Building* pBuilding, int level);
	Texture* m_pTextureConstructionSites = nullptr;
	Sprite m_pSpriteConstructionSiteSmall;
	Sprite m_pSpriteConstructionSiteLarge;
	std::unordered_map<std::type_index, std::unordered_map<Fraction, Texture*>> m_coastalConstructionTextures;



	enum class Ressources { Gold, Wood, Oil, Mana };
	Sprite* getUIElement(Ressources uiElement);
	Sprite getUISprite(const std::string& strPngPath);

	Animation getUnitIdleAnimation(Unit* pUnit, Direction direction);
	Animation getDyingAnimation(Unit* pUnit);
	Animation getBuildingFireAnimation(bool large) const;
	Animation getCorpseAnimation(const Unit* unit, Direction direction) const;


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
	Texture* m_pCorpseTexture = nullptr;
	std::map<SpellVisual, Animation> m_spellAnimations;
	Texture* m_pAirUnitShadowTexture = nullptr;
	Sprite m_airUnitShadow;
	std::unordered_map<ResourceType, TextureMap> m_mapWorkerResourceTexturesHuman;
	std::unordered_map<ResourceType, TextureMap> m_mapWorkerResourceTexturesOrc;

	//  Animation m_idleMage, m_idleBattleShip, m_idleGryphonRider;

	std::unordered_map<std::type_index, std::unordered_map<Direction, Animation>> mapIdleAnimations;
	std::unordered_map<std::type_index, std::unordered_map<Direction, Animation>> mapUnitMovementAnimations, mapUnitAttackAnimations, mapUnitDyingAnimations;



	const Sprite* getBuffIcon(BuffType type) const;

private:
	template<typename... Args>
	Texture* makeTexture(Args&&... args) {
		auto texture = std::make_unique<Texture>(std::forward<Args>(args)...);
		Texture* result = texture.get();
		m_ownedTextures.push_back(std::move(texture));
		return result;
	}

	void notifySeasonObservers() const;
	Season m_currentSeason = Season::Summer;
	std::vector<SeasonObserver*> m_vecSeasonObservers;
	std::vector<std::unique_ptr<Texture>> m_ownedTextures;
	std::map<BuffType, std::unique_ptr<Sprite>> m_buffSprites;
	std::unordered_map<std::string, Texture*> m_uiTextures;

	Texture* getUnitTexture(Unit* pUnit, ResourceType carriedResource);
	void createUnitSprites(Fraction fraction);
	void loadCoastalConstructionSprites();
	void createidleAnimations();
	void createidleAnimation(std::type_index correspondingType, int squareWidth_px, int rows, int cols, unsigned int interval_ms, Point posOffset);

	static const std::string strPrefix;



	// Icons -----------------------------------------------------
private:
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
	Texture* m_pSmallFireTexture = nullptr;
	Texture* m_pBigFireTexture = nullptr;
	Animation m_smallFireAnimation;
	Animation m_bigFireAnimation;

	// Icons -----------------------------------------------------



	// Buildings -----------------------------------------------------
	void readBuildings();
	void readBuildingSprite(Season season, const std::string& pathToFile, bool orcs);

	BuildingTextureMap m_mapBuildingTexturesHuman, m_mapBuildingTexturesOrc;
	std::unordered_map<std::type_index, std::unordered_map<PlayerColor, Texture*>> m_ActiveSeasonBuildingsHuman, m_ActiveSeasonBuildingsOrc;

	MultiBuildingTextureMap<Townhall::UpgradeLevel> m_mapTownhallTexturesHuman, m_mapTownhallTexturesOrc;
	MultiBuildingActiveSeason<Townhall::UpgradeLevel> m_ActiveSeasonTownhallHuman, m_ActiveSeasonTownhallOrc;

	MultiBuildingTextureMap<ScoutTower::UpgradeLevel> m_mapScoutTowerTexturesHuman, m_mapScoutTowerTexturesOrc;
	MultiBuildingActiveSeason<ScoutTower::UpgradeLevel> m_ActiveSeasonScoutTowerTexturesHuman, m_ActiveSeasonScoutTowerTexturesOrc;

	NeutralBuildingTextureMap m_mapNeutralBuildingTextures, m_mapActiveNeutralBuildingTextures;
	void switchBuildingSprites(Season season);
public:
	Sprite getBuildingSprite(Building* pBuilding, Fraction fraction, PlayerColor color, bool underConstruction = false);
	Sprite getBuildingPlacementSprite(PeasantActions action, Fraction fraction, PlayerColor color);
	Sprite getNeutralBuildingSprite(Building* pBuilding, int index = 0);
private:

	// Buildings -----------------------------------------------------

	BuildingDestroyedMap m_mapDestroyedSites;


	// TileSets --------------------------------------------------
	Texture* m_pActiveTileSet = nullptr;

	public:
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


	Renderer* m_pRenderer = nullptr;
};


extern ResourceManager_wartools* g_pRessourceManager;
