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

#include "Map.h"
#include "Units/IArmed.h"
#include "Global/Viewport.h"
#include "Global/DebugManager.h"
#include "Global/GlobalAttributes.h"
#include "Buildings/Barracks.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Church.h"
#include "Buildings/Farm.h"
#include "Buildings/Foundry.h"
#include "Buildings/Goldmine.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/LumberMill.h"
#include "Buildings/MageTower.h"
#include "Buildings/OilRefinery.h"
#include "Buildings/OilRig.h"

#include "Buildings/ScoutTower.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Stables.h"
#include "Buildings/Townhall.h"
#include "Buildings/DarkPortal.h"
#include "Units/Critter.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"
#include "Player.h"


#include "Units/LandUnits/Peasant/Peasant.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Footman.h"
#include "Units/LandUnits/Knight.h"
#include "Units/LandUnits/Mage.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"


#include <pud.h>
#include <cassert>
#include <algorithm>

#include <iostream>
using namespace std;


#include "Renderer.h"
#include "MapObject.h"


/*
 * Finds, if possible, a free Tile close to pTile which fulfills the following conditions:
 *
 * No land or sea unit on it, no lumber or rocks and no coast
 *
 * Returns nullptr if nothing could be found.
 */
Tile* Map::getNextFreeTile(const Point& world_pos, int maxDistance) {
	for (int distance = 1; distance <= maxDistance; distance++) {
		for (int dy = -distance; dy <= distance; dy++) {
			for (int dx = -distance; dx <= distance; dx++) {
				int y = world_pos.y + dy; // y vertical
				int x = world_pos.x + dx; // x horizontal

				if (dx == -distance || dx == distance || dy == -distance || dy == distance) {
					if (y >= 0 && y < m_arrTileMap.size() && x >= 0 && x < m_arrTileMap[0].size()) {
						Tile* nextTile = m_arrTileMap[y][x]; // Access via [y][x]

						if (!nextTile->getSeaOrLandUnit() && !nextTile->isDestroyable() && !nextTile->isCoast()) {
							return nextTile; // Return frist matching tile
						}
					}
				}
			}
		}
	}

	return nullptr;
}


Tile* Map::getNextFreeTile(Tile* pTile, int maxDistance) {
	Point pos_world = { pTile->getX(), pTile->getY() };
	return getNextFreeTile(pos_world, maxDistance);
}


std::vector<Tile*> Map::getNeighbourTiles(Tile* pTile, int distance) {
	std::vector<Tile*> vecNeighbourTiles;

	int tileX = pTile->getX();
	int tileY = pTile->getY();

	for (int dx = -distance; dx <= distance; ++dx) {
		for (int dy = -distance; dy <= distance; ++dy) {
			// Skip current tile
			if (dx == 0 && dy == 0)
				continue;

			int nx = tileX + dx;
			int ny = tileY + dy;

			// Are coordinates inside map?
			if (nx >= 0 && nx < m_arrTileMap[0].size() && ny >= 0 && ny < m_arrTileMap.size()) {
				Tile* neighbourTile = m_arrTileMap[ny][nx];
				vecNeighbourTiles.push_back(neighbourTile);
			}
		}
	}

	return vecNeighbourTiles;
}



Tile* Map::getNeighbourTileWithWood(Tile* pTile) {
	std::vector<Tile*> vecNeighbourTiles = getNeighbourTiles(pTile);

	// Use erase to remove any tiles without wood
	vecNeighbourTiles.erase(std::remove_if(vecNeighbourTiles.begin(), vecNeighbourTiles.end(),
		[](Tile* t) { return !t->isWood(); }),
		vecNeighbourTiles.end());

	if (vecNeighbourTiles.size())
		return vecNeighbourTiles.front();

	return nullptr;
}



void Map::processMapObjects() {
	m_vecMapObjects.erase(std::remove_if(m_vecMapObjects.begin(),
		m_vecMapObjects.end(),
		[](const MapObject* pObject) {
			if (pObject->isExpired()) {
				delete pObject;
				return true;
			}

			return false;
		}), m_vecMapObjects.end());


	// Process projectiles and similar
	for (auto& pObject : m_vecMapObjects) {
		pObject->process();
	}


	for (auto& [pTile, animation] : m_mapAnimations) {
		animation.process();
	}

	// Remove animations that played completely
	std::erase_if(m_mapAnimations, [](const auto& pair) {
		return pair.second.isComplete();
		});
}


void Map::notifySeasonChanged(Season newSeason) {
	createMiniMap();
}


void Map::renderMiniMapOverlay(Viewport* pViewPort) {
	assert(m_pTextureMiniMap);
	delete m_pTextureMiniMapWithOverlay;

	SDL_Renderer* renderer = g_pRenderer->accessSDLRenderer();

	// Step 1: Create a clean copy of the minimap
	SDL_Texture* pTexCleanMiniMap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_pTextureMiniMap->getWidth(), m_pTextureMiniMap->getHeight());

	SDL_SetRenderTarget(renderer, pTexCleanMiniMap);
	SDL_RenderCopy(renderer, m_pTextureMiniMap->getSDLTexture(), nullptr, nullptr);

	// Step 2: Render overlay into
	SDL_SetRenderTarget(renderer, pTexCleanMiniMap);

	float realMapLength_px = (float)(m_arrTileMap.size() * Tile::TileWidth_px);
	float miniMapLength_px = (float)m_pTextureMiniMap->getWidth(); // Or height, does not matter -> is squared
	float scaleFactor = miniMapLength_px / realMapLength_px;

	for (auto& e : m_mapPlayer) {
		Player* pPlayer = e.second;
		std::vector<Object*>& vecObjects = pPlayer->getObjects();

		for (auto& pObject : vecObjects) {
			if (!pObject->isPlacedOnMap())
				continue;

			Point pos = pObject->getTileLocation();
			pos.x = (int)(pos.x * Tile::TileWidth_px * scaleFactor);
			pos.y = (int)(pos.y * Tile::TileWidth_px * scaleFactor);

			int unitTileSize = gMapUnitTilesize[typeid(*pObject)];
			Rect r = { pos.x, pos.y, (int)(unitTileSize * Tile::TileWidth_px * scaleFactor), (int)(unitTileSize * Tile::TileWidth_px * scaleFactor) };


			if (pObject->getFraction() != Fraction::Neutral) {
				const SDL_Color& c = mapPlayerPaletteColors.at(pObject->getOwner()->getColor()).front(); // Simply take first color
				g_pRenderer->drawFilledRect(r, c.r, c.g, c.b, 255);
			}
			else {
				g_pRenderer->drawFilledRect(r, 255, 255, 0, 255);
			}
		}
	}

	// Step 3: Render scaled viewport 
	Point viewPort_TL_px = pViewPort->getCurrentPosition_TL_tile();
	viewPort_TL_px.x = (int)(viewPort_TL_px.x * Tile::TileWidth_px * scaleFactor);
	viewPort_TL_px.y = (int)(viewPort_TL_px.y * Tile::TileWidth_px * scaleFactor);

	Rect rectScaledViewPort = { viewPort_TL_px.x, viewPort_TL_px.y,
		(int)((pViewPort->getMaxViewableMapWidth_tiles() * Tile::TileWidth_px * scaleFactor) - viewPort_TL_px.x),
		(int)((pViewPort->getMaxViewableMapHeight_tiles() * Tile::TileWidth_px * scaleFactor) - viewPort_TL_px.y) };
	g_pRenderer->drawRect(rectScaledViewPort, 255, 255, 255);

	// Step 4: Store result
	m_pTextureMiniMapWithOverlay = new Texture(pTexCleanMiniMap, m_pTextureMiniMap->getWidth(), m_pTextureMiniMap->getHeight());
	SDL_SetRenderTarget(renderer, nullptr);
}


void Map::createMiniMap() {
	delete m_pTextureMiniMap; // Release minimap if already existend
	SDL_Renderer* renderer = g_pRenderer->accessSDLRenderer();

	// Step 1: Render complete map into a texture which is big enough
	int tile_cnt = (int)m_arrTileMap.size();
	SDL_Texture* pTexBigCompleteMap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tile_cnt * Tile::TileWidth_px, tile_cnt * Tile::TileWidth_px);

	// Use renderer to render in texTarget
	SDL_SetRenderTarget(renderer, pTexBigCompleteMap);
	SDL_RenderClear(renderer);

	// Render complete map
	for (int y = 0; y < tile_cnt; ++y) {
		for (int x = 0; x < tile_cnt; ++x) {
			Tile* pTile = m_arrTileMap[y][x];

			Rect r;

			// funktionen zusammenfassen??
			r.x = x * Tile::TileWidth_px;
			r.y = y * Tile::TileWidth_px;
			r.w = Tile::TileWidth_px;
			r.h = Tile::TileWidth_px;

			Point p = { r.x, r.y };

			Rect r1 = { 0,0, 32,32 };

			g_pRenderer->renderSprite(p, pTile->getSprite());
		}
	}

	// Step 2: Create the actual minimap by rendering pTexBigCompleteMap into pTexActualMiniMap. SDL2 takes care about scaling!
	const int minimap_px = 300;
	SDL_Texture* pTexActualMiniMap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, minimap_px, minimap_px);
	SDL_SetRenderTarget(renderer, pTexActualMiniMap);
	SDL_RenderCopy(renderer, pTexBigCompleteMap, nullptr, nullptr);

	m_pTextureMiniMap = new Texture(pTexActualMiniMap, minimap_px, minimap_px);

	// Step 3: Release the big texture as it is not needed anymore
	SDL_DestroyTexture(pTexBigCompleteMap);

	// Release renderer    
	SDL_SetRenderTarget(renderer, nullptr);
}


Map::Map(const std::string& filename, ResourceManager_wartools* pResManagerWartools) {
	m_pResManagerWartools = pResManagerWartools;
	pResManagerWartools->registerSeasonObserver(this);
	Pud* pPud = pud_open(filename.c_str(), PUD_OPEN_MODE_R);

	if (!pPud) {
		throw std::logic_error("Could not open PUD file");
	}

	//Todo: w und h vertauschen damit konsistent (wenn auch gleich da quadratisch)
	// Puds are always squares. Hence map w==h
	m_arrTileMap.resize(pPud->map_h);
	for (std::size_t y = 0; y < pPud->map_h; ++y) {
		m_arrTileMap[y].resize(pPud->map_w);
	}

	unsigned int tx = 0, ty = 0;
	for (unsigned int i = 0; i < pPud->tiles; ++i) {
		Tile* pTile = new Tile(tx, ty, pPud->tiles_map[i]);
		m_arrTileMap[ty][tx] = pTile;

		if (++tx >= pPud->map_w) {
			tx = 0;
			++ty;
		}
	}

	Tile::m_pMap = this;

	identifyNeighbourTiles();

	addPlayersToMap(pPud);
	addUnitsToMap(pPud);
	createMiniMap();

	Season s = (Season)pPud->era;
	pResManagerWartools->switchSeason(s);


	// Debug ------------------------------------

		/*
		Player* pOwner = m_mapPlayer[PlayerColor::Red];
		Peasant* pPeasant = new Peasant(pOwner, Fraction::Alliance, this);
		placeUnit(m_arrTileMap[0][0], pPeasant);
		Point targetPos = {13,35};
		pPeasant->harvestResource(targetPos);

		*/
		// ---------------------------------------


		/*
	   std::cout<<"Tag        : "<<pud->tag<<std::endl;
	   std::cout<<"Version    : "<<pud->version<<std::endl;
	   std::cout<<"Description: "<<pud_description_get(pud)<<std::endl;

	   */

	   /*
	  PUD_DIMENSIONS_UNDEFINED = 0,
		PUD_DIMENSIONS_32_32,
		PUD_DIMENSIONS_64_64,
		PUD_DIMENSIONS_96_96,
		PUD_DIMENSIONS_128_128
		 std::cout<<"Dimensions : "<<pud->dims<<", w="<<pud->map_w<<", h="<<pud->map_h<<" ("<<pud->tiles<<" Tiles)"<<std::endl;
		*/



		/*
			 // pud->h Pud_Upgrade struktur
			 // Upgrades http://classic.battle.net/war2/units/archer.shtml

			std::cout<<"Upgrades: "<<std::endl;
			for(std::size_t i=0; i<52; ++i) {
				std::cout<<'\t'<<"Upgrade "<<std::dec<<i<<std::endl;
				std::cout<<"\t\tTime  : "<<(int)pud->upgrades[i].time<<std::endl;
				std::cout<<"\t\tGold  : "<<(int)pud->upgrades[i].gold<<std::endl;
				std::cout<<"\t\tLumber: "<<(int)pud->upgrades[i].lumber<<std::endl;
				std::cout<<"\t\tOil   : "<<(int)pud->upgrades[i].oil<<std::endl;
				std::cout<<"\t\tIcon  : "<<(int)pud->upgrades[i].icon<<std::endl;
				std::cout<<"\t\tGroup : "<<(int)pud->upgrades[i].group<<std::endl;
				std::cout<<"\t\tFlags : "<<std::hex<<(int)pud->upgrades[i].flags<<std::endl;
			}

		  //  std::cout<<std::dec<<pud->tiles<<std::endl;
		//std::cin.get();
			for(std::size_t i=0; i<pud->tiles; ++i) {
		//        std::cout<<std::dec<<i+1<<"\t"<<std::hex<<pud->tiles_map[i]<<std::endl;
			}

		}
	*/
}


Map::~Map() {
	m_pResManagerWartools->deregisterSeasonObserver(this);

	for (auto& e : m_mapPlayer) {
		Player* pPlayer = e.second;
		delete pPlayer;
	}

	for (int y = 0; y < sideLength(); ++y) {
		for (int x = 0; x < sideLength(); ++x) {
			delete m_arrTileMap[y][x];
		}
	}
}


void Map::removeObjects() {
	for (auto& e : m_mapPlayer) {
		Player* pPlayer = e.second;
		std::vector<Object*>& vecObjects = pPlayer->getObjects();

		for (auto it = vecObjects.begin(); it != vecObjects.end(); ) {
			if ((*it)->isDead()) {
				delete* it;
				it = vecObjects.erase(it);
			}
			else {
				++it;
			}
		}
	}
}



std::set<Object*> Map::getObjects(const Point& tileUL_world, const Point& tileLR_world) {
	/*
	 * Buildings are spread over several tiles, e.g. 3x3.
	 * We want to return just one pointer to each Object in any case -> std::set
	 */
	std::set<Object*> setObjects;

	for (int x = tileUL_world.x; x <= tileLR_world.x; ++x) {
		for (int y = tileUL_world.y; y <= tileLR_world.y; ++y) {
			Object* pObj = getObjectOnTile(x, y);
			if (pObj)
				setObjects.insert(pObj);
		}
	}

	return setObjects;
}


Object* Map::getObjectOnTile(int tile_x_world, int tile_y_world) {
	tile_x_world = std::max(0, tile_x_world);
	tile_x_world = std::min(tile_x_world, sideLength() - 1);

	tile_y_world = std::max(0, tile_y_world);
	tile_y_world = std::min(tile_y_world, sideLength() - 1);

	// Is the tile explored / visible?
	auto& mapExplored = humanControlledPlayer()->getMapInfo()->m_mapExplored;
	auto& mapCurrentlyVisible = humanControlledPlayer()->getMapInfo()->m_mapCurrentlyVisible;

	if (gDebugManager.m_map_considerExploration && !mapExplored[tile_y_world][tile_x_world])
		return nullptr;

	if (gDebugManager.m_map_drawFogOfWar && !mapCurrentlyVisible[tile_y_world][tile_x_world])
		return nullptr;

	// Yes, the player can actually see the tile he clicked on
	Tile* pTile = m_arrTileMap[tile_y_world][tile_x_world];

	if (pTile->getAirUnit())
		return pTile->getAirUnit();

	if (pTile->getSeaOrLandUnit())
		return pTile->getSeaOrLandUnit();

	return nullptr;
}


Object* Map::getObjectOnTile(const Point& tile_world) {
	return getObjectOnTile(tile_world.x, tile_world.y);
}


// ToDo: Implement for circle
// ToDo: Cache result? Maybe called several times per frame, by an object, by the minimap, ...
void Map::getVisibleTiles(Object* pObj, int sightRange, std::vector<Tile*>& vecVisibleTiles) const {
	assert(pObj->isPlacedOnMap());

	// ToDo: Improve, currently Rectangle shape

	// We have to consider that Object can be a building occupying, e.g., 2x2 Tiles.
	const Tile* pTileTopLeft = pObj->getTileTopLeft();
	const Tile* pTileLowerRight = pObj->getTileLowerRight();


	// Upper left corner
	int posX_upperLeft_tile = std::max(pTileTopLeft->getPos().x - sightRange, 0);
	int posY_upperLeft_tile = std::max(pTileTopLeft->getPos().y - sightRange, 0);

	// Lower right corner
	int posX_lowerRight_tile = std::min(pTileLowerRight->getPos().x + sightRange, sideLength() - 1);
	int posY_lowerRight_tile = std::min(pTileLowerRight->getPos().y + sightRange, sideLength() - 1);

	// Extract all tiles within the rectangle
	for (int y = posY_upperLeft_tile; y < posY_lowerRight_tile; ++y) {
		for (int x = posX_upperLeft_tile; x < posX_lowerRight_tile; ++x) {
			Tile* pTile = m_arrTileMap[y][x];
			vecVisibleTiles.push_back(pTile);
		}
	}
}


// ToDo: Overload with Point parameter instead of Object pointer
/*
 * Returns all visible objects for a given Object. This vector can be filtered
 * for enemy, neutral or alliance units afterwards.
 *
 * Note: The returned set will NOT include pObj (the object itself) which of course is
 * within the visible range.
 *
 * This function returns a set. Consider a Townhall building which occupies 3x3 tiles.
 * Using a sets avoids returning 9 Pointers for the same object.
 */

 /*
  * Range might be the object's sight, its attack range
  * (which might be smaller than the sight) or any other desired value
  */
std::set<Object*> Map::getVisibleObjectsWithinRange(Object* pObj, int range, bool includeAirUnits, bool includeDeadUnits) const {
	std::vector<Tile*> vecVisibleTiles;
	getVisibleTiles(pObj, range, vecVisibleTiles);

	std::set<Object*> setVisibleObjects;
	for (auto& pTile : vecVisibleTiles) {
		// Dead land units can be resurrected by death knights
		if (pTile->getSeaOrLandUnit() && (!pTile->getSeaOrLandUnit()->isDead() || includeDeadUnits)) {
			setVisibleObjects.insert(pTile->getSeaOrLandUnit());
		}

		if (includeAirUnits && pTile->getAirUnit() && !pTile->getAirUnit()->isDead()) {
			setVisibleObjects.insert(pTile->getAirUnit());
		}
	}

	setVisibleObjects.erase(pObj);

	return setVisibleObjects;
}


bool Map::isSpaceAvailableForUnit(int tile_y, int tile_x, int unitTileSize, Object::Type objType) {
	int max_x = tile_x + unitTileSize - 1;
	int max_y = tile_y + unitTileSize - 1;

	if (max_x >= sideLength())
		return false;

	if (max_y >= sideLength())
		return false;

	// bei townhall z.b. auf abstand zu goldminen in der nähe pruefen
	for (int y = tile_y; y <= max_y; ++y) {
		for (int x = tile_x; x <= max_x; ++x) {
			Tile* pTile = m_arrTileMap[y][x];

			if (objType == Object::Type::Air && pTile->getAirUnit()) {
				return false;
			}
			else if (objType == Object::Type::Land && (!pTile->isLand() || pTile->isDestroyable() || pTile->getSeaOrLandUnit())) {
				return false;
			}
			else if (objType == Object::Type::Sea && (!pTile->isSea() || pTile->getSeaOrLandUnit())) {
				return false;
			}
		}
	}

	return true;
}


/*
 * Can a specified unit type be placed directly beside our building?
 * Returns exactly one tile which can be used to place the unit. If the unit dimensions exceed
 * 1x1 Tile* represents the topper left tile which can be used to place the unit with Map::placeUnit().
 *
 * Note: More places might be available but the algorithm stops after a valid tile has been found.
 *
 * If Tile* is nullptr then placing the desired unit is not possible.
 */
Tile* Map::getFreeTileAroundBuilding(Object* pBuilding, const Object* pObject) {
	int unit_tilesize = gMapUnitTilesize[typeid(*pObject)];
	return getFreeTileAroundBuilding(pBuilding, unit_tilesize, pObject->getType());
}


Tile* Map::getFreeTileAroundBuilding(Object* pBuilding, int unit_tilesize, Object::Type landSeaAir) {
	int searchPos_x_TL = std::max<int>(0, pBuilding->getTileTopLeft()->getPos().x - unit_tilesize);
	int searchPos_y_TL = std::max<int>(0, pBuilding->getTileTopLeft()->getPos().y - unit_tilesize);

	int searchPos_x_LR = std::min<int>(sideLength() - 1, pBuilding->getTileLowerRight()->getPos().x + unit_tilesize + 1);
	int searchPos_y_LR = std::min<int>(sideLength() - 1, pBuilding->getTileLowerRight()->getPos().y + unit_tilesize + 1);


	for (int y = searchPos_y_TL; y < searchPos_y_LR; ++y) {
		for (int x = searchPos_x_TL; x < searchPos_x_LR; ++x) {
			if (isSpaceAvailableForUnit(y, x, unit_tilesize, landSeaAir)) {
				return m_arrTileMap[y][x];
			}
		}
	}

	return nullptr;
}



// Removes pObject from its current tiles and places it at pTile. pTile is the new upper left tile (important for unit size bigger than 1)
void Map::placeUnit(Tile* pTile, Object* pObject) {
	assert(pObject);
	placeUnit(pTile->getPos().y, pTile->getPos().x, pObject);
}


/*
 * Places a given Object on the map.
 *
 * Note: You still have to add the object to the appropriate player object list
 *
 * Important:
 * This function assumes that the caller has checked whether all required tiles
 * have the valid type (land units on a land tile) and that they are not occupied
 * by another person and within the map dimensions.
 *
 * Note: Check this with getFreeTile / isSpaceAvailableForUnit
 */
void Map::placeUnit(int tile_y, int tile_x, Object* pObject) {
	/*
	 * If the unit is not placed for the first time we have to
	 * release the current tile assignments.
	 */
	for (auto& pTile : pObject->getOccupiedTiles()) {
		pTile->release(pObject);
	}

	pObject->getOccupiedTiles().clear();


	assert(gMapUnitTilesize.contains(typeid(*pObject)));
	int tilesize = gMapUnitTilesize[typeid(*pObject)];

	for (int y = tile_y; y < tile_y + tilesize; ++y) {
		for (int x = tile_x; x < tile_x + tilesize; ++x) {
			Tile* pTile = m_arrTileMap[y][x];
			pTile->assign(pObject);
			pObject->getOccupiedTiles().push_back(pTile);
		}
	}
}


// mapState: Give information about which tiles are free and which are not-free
bool Map::canBePlacedAtPosition(Object* pObject, int tile_x, int tile_y, std::unordered_map<Tile*, bool>* mapState) {
	assert(gMapUnitTilesize.contains(typeid(*pObject)));
	int tilesize = gMapUnitTilesize[typeid(*pObject)];

	bool canBePlaced = true;

	for (int y = tile_y; y < tile_y + tilesize; ++y) {
		for (int x = tile_x; x < tile_x + tilesize; ++x) {
			Tile* pTile = m_arrTileMap[y][x];

			bool tileIsBlocked = pTile->isBlockedForUnit(pObject);
			if (tileIsBlocked && !mapState)
				return false;

			if (tileIsBlocked && mapState) {
				canBePlaced = false;
				(*mapState)[pTile] = false;
			}
		}
	}

	return canBePlaced;
}


void Map::releaseObjectFromTile(Object* pObject) {
	assert(pObject);

	for (auto& pTile : pObject->getOccupiedTiles()) {
		pTile->release(pObject);
	}

	pObject->getOccupiedTiles().clear();
}


/*
 * Can our unit placed directly beside our building?
 * Returns exactly one place (consisting of 'unitSize_tiles' tiles) which
 * can be used to place the unit. More places might be available but are not
 * included within the return vector.
 *
 * Empty vector: Unit can't be placed directly beside our building
 */



 /*
  * This function is called once if a map is created. Hence, all neighbours of a Tile
  * are just calculated once and stored for later access.
  */
void Map::identifyNeighbourTiles() {
	for (std::size_t y = 0; y < m_arrTileMap.size(); ++y) {
		for (std::size_t x = 0; x < m_arrTileMap[y].size(); ++x) {
			NeighbourNodes neighbours;

			Tile* pTile = m_arrTileMap[y][x];
			// Identify all sourrunding neighbour Tiles for pTile...
			// 1. Row: N|N|N
			// 2. Row: N|T|N
			// 3. Row: N|N|N
			const Point& tile_pos = pTile->getPos();
			int min_x = tile_pos.x - 1;
			int max_x = tile_pos.x + 1;

			int min_y = tile_pos.y - 1;
			int max_y = tile_pos.y + 1;

			// ... but consider map boundaries since x and y might be smaller than 0 or bigger than Map::sideLength()
			// 1. Row
			if (min_y >= 0) {
				if (min_x >= 0) {
					neighbours[Direction::NW] = m_arrTileMap[min_y][min_x];
				}

				neighbours[Direction::N] = m_arrTileMap[min_y][tile_pos.x];

				if (max_x < sideLength()) {
					neighbours[Direction::NE] = m_arrTileMap[min_y][max_x];
				}
			}

			// 2. Row
			if (min_x >= 0) {
				neighbours[Direction::W] = m_arrTileMap[tile_pos.y][min_x];
			}

			if (max_x < sideLength()) {
				neighbours[Direction::E] = m_arrTileMap[tile_pos.y][max_x];
			}

			// 3. Row
			if (max_y < sideLength()) {
				if (min_x >= 0) {
					neighbours[Direction::SW] = m_arrTileMap[max_y][min_x];
				}

				neighbours[Direction::S] = m_arrTileMap[max_y][tile_pos.x];

				if (max_x < sideLength()) {
					neighbours[Direction::SE] = m_arrTileMap[max_y][max_x];
				}
			}


			NeighbourNodes neighbours_SameType(neighbours);

			/*
			neighbours_SameType.erase(std::remove_if(neighbours_SameType.begin(),
						   neighbours_SameType.end(),
						   [&pTile](const auto& it) {
					const Tile* pNeighbour = dynamic_cast<Tile*>(it.second);
				if(pTile->isSea() && pNeighbour->isSea())
					return false;
				else if(pTile->isLand() && pNeighbour->isLand())
					return false;

				return true; // Delete if Tile type does not match or tile is coastal etc.
			}), neighbours_SameType.end());
			*/



			std::erase_if(neighbours_SameType, [&pTile](const auto& pair) {
				const Tile* pNeighbour = (Tile*)pair.second;
				if (pTile->isSea() && pNeighbour->isSea())
					return false;
				else if (pTile->isLand() && pNeighbour->isLand())
					return false;

				return true; // Delete if Tile type does not match or tile is coastal etc
				});


			pTile->neighbourTiles = neighbours;
			pTile->neighbourTiles_SameType = neighbours_SameType;
		}
	}
}


void Map::addPlayersToMap(Pud* pPud) {
	// Neutral player is always existent (Critter, Goldmines, ...)
	m_mapPlayer[PlayerColor::Neutral] = new Player("None", PlayerColor::Neutral, Fraction::Neutral, new MapInfo(*this));
	std::cout << "addPlayersToMap: " << std::endl;

	std::vector<PlayerColor> vecPlayerColors;

	std::cout << "\nPlayers (3: Nobody, 5: Human 6: Rescuable (passiv)): " << std::endl;
	for (std::size_t i = 0; i < 8; ++i) {
		std::cout << '\t' << std::hex << (int)pPud->owner.players[i] << std::endl;

		if ((int)pPud->owner.players[i] != 3) {
			vecPlayerColors.push_back((PlayerColor)i);
		}
	}

	// Todo: What about AI players...

	std::cout << "\nUnusable: " << std::endl;
	for (std::size_t i = 0; i < 7; ++i) {
		std::cout << '\t' << std::hex << (int)pPud->owner.unusable[i] << std::endl;
	}


	for (unsigned int i = 0; i < pPud->starting_points; ++i) {
		PlayerColor color = vecPlayerColors[i];
		Resources res((int)pPud->sgld.players[i], (int)pPud->slbr.players[i], (int)pPud->soil.players[i]);

		// Todo: get real fraction from pud
		Fraction fraction = (bool)pPud->side.players[i] ? Fraction::Orc : Fraction::Alliance;


		/*
		 * Blackrock Clan Red Stromgarde
		 * Stormreaver Clan Blue  Azeroth
		 * Twilight's Hammer Clan Violet Dalaran
		 * Black Tooth Grin Clan    Black Gilneas
		 * Bleeding Hollow Clan Green Kul Tiras
		 * Dragonmaw Clan White Lordaeron
		 * Burning Blade Orange Alterac
		 */
		m_mapPlayer[color] = new Player("Player " + std::to_string((int)color), color, fraction, new MapInfo(*this), res);
	}

	// Hack
   // m_mapPlayer[PlayerColor::Blue]->m_fraction = Fraction::Orc;
}


// Add units (including critter and buildings)
void Map::addUnitsToMap(Pud* pPud) {
	// Owner: 0 bis 3, 0x0f -> Critter (neutral)
	for (std::size_t i = 0; i < pPud->units_count; ++i) {
		Pud_Unit_Info& ui = pPud->units[i];

		// ToDo: check if owner 0,7 und 15
		/*
		* Each of the fiels within this structure are of type Pud_Owner
		* This structure is affected by the War2 defaults.

	   struct {
		  uint8_t players[8];  Real players (from 1 to 8)
		  uint8_t unusable[7]; Phantom players
		  uint8_t neutral; Neutral player (critters, dark portal, gold mine, ...)
	   } owner;
	   */



		PlayerColor color = (PlayerColor)ui.player;

		//  std::cout<<"Player id: "<<std::dec<<(int)ui.owner<<"\t"<<(int)static_cast<PlayerColor>(ui.owner)<<std::endl;
		//  PlayerColor color = static_cast<PlayerColor>(ui.owner);

		if (!m_mapPlayer.contains(color))
			throw std::logic_error("Player color not in map!");

		Player* pOwner = m_mapPlayer[color];

		switch (ui.type) {
			// Start points
		case PUD_UNIT_HUMAN_START:
			if (pOwner == humanControlledPlayer()) {
				m_humanPlayerStartPosition.x = ui.x;
				m_humanPlayerStartPosition.y = ui.y;
			}
			break;

		case PUD_UNIT_ORC_START:
			if (pOwner == humanControlledPlayer()) {
				m_humanPlayerStartPosition.x = ui.x;
				m_humanPlayerStartPosition.y = ui.y;
			}
			break;

			// Resources            
		case PUD_UNIT_GOLD_MINE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GoldMine(m_mapPlayer[PlayerColor::Neutral], this, ui.alter * 1000));
			break;

		case PUD_UNIT_DARK_PORTAL:
			placeUnit(m_arrTileMap[ui.y][ui.x], new DarkPortal(m_mapPlayer[PlayerColor::Neutral], this));
			break;


		case PUD_UNIT_OIL_PATCH:
			break;

			// Neutral
		case PUD_UNIT_CRITTER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Critter(m_mapPlayer[PlayerColor::Neutral], this));
			break;


		case PUD_UNIT_SKELETON:
			//     placeUnit(m_arrTileMap[ui.y][ui.x], new Skeleton(pOwner, this));
			break;




		case PUD_UNIT_HUMAN_OIL_WELL:
			std::cerr << "oil well " << std::dec << ui.alter << std::endl;
			placeUnit(m_arrTileMap[ui.y][ui.x], new OilRig(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_OIL_WELL:
			placeUnit(m_arrTileMap[ui.y][ui.x], new OilRig(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_CHURCH:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Church(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ALTAR_OF_STORMS:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Church(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_HUMAN_SHIPYARD:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Shipyard(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_SHIPYARD:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Shipyard(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_GRYPHON_AVIARY:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GryphonAviary(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_DRAGON_ROOST:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GryphonAviary(pOwner, Fraction::Orc, this));
			break;



		case PUD_UNIT_HUMAN_BARRACKS:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Barracks(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_BARRACKS:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Barracks(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_HUMAN_BLACKSMITH:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Blacksmith(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_BLACKSMITH:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Blacksmith(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_FARM:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Farm(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_PIG_FARM:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Farm(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_ELVEN_LUMBER_MILL:
			placeUnit(m_arrTileMap[ui.y][ui.x], new LumberMill(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_TROLL_LUMBER_MILL:
			placeUnit(m_arrTileMap[ui.y][ui.x], new LumberMill(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_MAGE_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new MageTower(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_TEMPLE_OF_THE_DAMNED:
			placeUnit(m_arrTileMap[ui.y][ui.x], new MageTower(pOwner, Fraction::Orc, this));
			break;



		case PUD_UNIT_STABLES:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Stables(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_OGRE_MOUND:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Stables(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_GNOMISH_INVENTOR:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GnomishInventor(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_GOBLIN_ALCHEMIST:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GnomishInventor(pOwner, Fraction::Orc, this));
			break;





		case PUD_UNIT_HUMAN_FOUNDRY:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Foundry(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_FOUNDRY:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Foundry(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_HUMAN_REFINERY:
			placeUnit(m_arrTileMap[ui.y][ui.x], new OilRefinery(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_REFINERY:
			placeUnit(m_arrTileMap[ui.y][ui.x], new OilRefinery(pOwner, Fraction::Orc, this));
			break;

			// ScoutTower

		case PUD_UNIT_HUMAN_SCOUT_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new ScoutTower(pOwner, Fraction::Alliance, this, ScoutTower::UpgradeLevel::ScoutTower));
			break;

		case PUD_UNIT_ORC_SCOUT_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new ScoutTower(pOwner, Fraction::Orc, this, ScoutTower::UpgradeLevel::ScoutTower));
			break;

		case PUD_UNIT_HUMAN_GUARD_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new ScoutTower(pOwner, Fraction::Alliance, this, ScoutTower::UpgradeLevel::GuardTower));
			break;

		case PUD_UNIT_ORC_GUARD_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new ScoutTower(pOwner, Fraction::Orc, this, ScoutTower::UpgradeLevel::GuardTower));
			break;

		case PUD_UNIT_HUMAN_CANNON_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new ScoutTower(pOwner, Fraction::Alliance, this, ScoutTower::UpgradeLevel::CannonTower));
			break;

		case PUD_UNIT_ORC_CANNON_TOWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new ScoutTower(pOwner, Fraction::Orc, this, ScoutTower::UpgradeLevel::CannonTower));
			break;




			// Townhall
			//   case PUD_UNIT_GREAT_HALL:
			//        placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Orc, pResManager, this, Townhall::UpgradeLevel_Townhall));
			//       break;

		case PUD_UNIT_TOWN_HALL:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Alliance, this, Townhall::UpgradeLevel::Townhall));
			break;


		case PUD_UNIT_KEEP:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Alliance, this, Townhall::UpgradeLevel::Keep));
			break;


		case PUD_UNIT_CASTLE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Alliance, this, Townhall::UpgradeLevel::Castle));
			break;


		case PUD_UNIT_GREAT_HALL:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Orc, this, Townhall::UpgradeLevel::Townhall));
			break;


		case PUD_UNIT_STRONGHOLD:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Orc, this, Townhall::UpgradeLevel::Keep));
			break;



		case PUD_UNIT_FORTRESS:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Townhall(pOwner, Fraction::Orc, this, Townhall::UpgradeLevel::Castle));
			break;



			// Flying units
		case PUD_UNIT_GNOMISH_FLYING_MACHINE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GnomishFlyingMaschine(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_GOBLIN_ZEPPLIN:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GnomishFlyingMaschine(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_GRYPHON_RIDER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GryphonRider(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_DRAGON:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GryphonRider(pOwner, Fraction::Orc, this));
			break;


			// Sea units
		case PUD_UNIT_BATTLESHIP:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Battleship(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_JUGGERNAUGHT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Battleship(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_ELVEN_DESTROYER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Destroyer(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_TROLL_DESTROYER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Destroyer(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_GNOMISH_SUBMARINE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GnomishSubmarine(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_GIANT_TURTLE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new GnomishSubmarine(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_HUMAN_TANKER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Tanker(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_TANKER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Tanker(pOwner, Fraction::Orc, this));
			break;


		case PUD_UNIT_HUMAN_TRANSPORT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Transport(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_ORC_TRANSPORT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Transport(pOwner, Fraction::Orc, this));
			break;




			// Land units
		case PUD_UNIT_PEASANT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Peasant(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_PEON:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Peasant(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_FOOTMAN:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Footman(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_GRUNT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Footman(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_ARCHER:
		{
			Archer* pArcher = new Archer(pOwner, Fraction::Alliance, this);
			placeUnit(m_arrTileMap[ui.y][ui.x], pArcher);
		}
		break;

		case PUD_UNIT_AXETHROWER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Archer(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_RANGER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Archer(pOwner, Fraction::Alliance, this, true));
			break;

		case PUD_UNIT_BERSERKER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Archer(pOwner, Fraction::Orc, this, true));
			break;

		case PUD_UNIT_KNIGHT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Knight(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_OGRE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Knight(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_PALADIN:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Knight(pOwner, Fraction::Alliance, this, true));
			break;

		case PUD_UNIT_OGRE_MAGE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Knight(pOwner, Fraction::Orc, this, true));
			break;

		case PUD_UNIT_BALLISTA:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Ballista(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_CATAPULT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Ballista(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_DWARVES:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Dwarfes(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_GOBLIN_SAPPER:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Dwarfes(pOwner, Fraction::Orc, this));
			break;

		case PUD_UNIT_MAGE:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Mage(pOwner, Fraction::Alliance, this));
			break;

		case PUD_UNIT_DEATH_KNIGHT:
			placeUnit(m_arrTileMap[ui.y][ui.x], new Mage(pOwner, Fraction::Orc, this));
			break;



		default:
			std::cout << "Unit not yet implemented: " << std::hex << ui.type << std::dec << std::endl;
		}


		std::cout << std::dec << i + 1 << "\tPosition: " << ui.x << "," << ui.y << "\tPlayer: " << std::hex << (int)ui.player << "\tType: " << std::hex << (int)ui.type << "\t" << ui.alter << std::endl;
	}
}


void Map::processPlayers() {
	for (auto& e : m_mapPlayer) {
		Player* pPlayer = e.second;
		pPlayer->process();
	}
}



void Map::draw(Renderer* pRenderer, Viewport* pViewPort) {
	Player* pHumanPlayer = humanControlledPlayer();

	auto& mapExplored = pHumanPlayer->getMapInfo()->m_mapExplored;
	auto& mapCurrentlyVisible = pHumanPlayer->getMapInfo()->m_mapCurrentlyVisible;

	// Render only tiles located in viewport and check for boundaries (never increment x and y >= m_arrTileMap.size())
	for (std::size_t y = pViewPort->getCurrentPosition_TL_tile().y; y < pViewPort->getMaxViewableMapHeight_tiles(); ++y) {
		for (std::size_t x = pViewPort->getCurrentPosition_TL_tile().x; x < pViewPort->getMaxViewableMapWidth_tiles(); ++x) {
			Tile* pTile = m_arrTileMap[y][x];

			if (gDebugManager.m_map_considerExploration && !mapExplored[y][x]) {
				Rect r;

				// funktionen zusammenfassen??
				r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
				r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
				r.w = Tile::TileWidth_px;
				r.h = Tile::TileWidth_px;

				// Black non-transparent filled rectangle
				pRenderer->drawFilledRect(r, 0, 0, 0, 0);
				continue;
			}


			Point posLocal_tile = pViewPort->worldToScreen_tile(pTile->getPos());

			if (pTile->getSprite().isValid())
				pRenderer->renderSprite(pViewPort->tileToPixel(posLocal_tile), pTile->getSprite());


			if (gDebugManager.m_map_drawFogOfWar && !mapCurrentlyVisible[y][x]) {
				Rect r;

				// funktionen zusammenfassen??
				r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
				r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
				r.w = Tile::TileWidth_px;
				r.h = Tile::TileWidth_px;

				// Gray non-filled rectangle
				pRenderer->drawRect(r, 127, 127, 127);
			}
		}
	}


	if (gDebugManager.m_map_drawTileInformation) {
		for (std::size_t y = pViewPort->getCurrentPosition_TL_tile().y; y < pViewPort->getMaxViewableMapHeight_tiles(); ++y) {
			for (std::size_t x = pViewPort->getCurrentPosition_TL_tile().x; x < pViewPort->getMaxViewableMapWidth_tiles(); ++x) {
				Tile* pTile = m_arrTileMap[y][x];

				if (pTile->isWood()) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = 32;
					r.h = 32;
					pRenderer->drawFilledRect(r, 0, 255, 0, 220);
				}
				else if (pTile->isLand()) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = 32;
					r.h = 32;
					pRenderer->drawFilledRect(r, 0, 255, 0, 130);
				}
				else if (pTile->isSea()) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = 32;
					r.h = 32;
					pRenderer->drawFilledRect(r, 0, 0, 255, 130);
				}
				else if (pTile->isCoast()) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = 32;
					r.h = 32;
					pRenderer->drawFilledRect(r, 127, 0, 255, 220); // Higher -> less alpha
				}

				if (pTile->isDestroyable()) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = 32;
					r.h = 32;
					pRenderer->drawFilledRect(r, 255, 165, 0, 200);
				}


				if (pTile->hasObject()) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = 32;
					r.h = 32;

					pRenderer->drawFilledRect(r, 255, 255, 0, 130);
				}
			}
		}
	}



	// Draw units
	// ToDo: Figures first, then buildings? Check that...
	std::vector<Object*> vecVisibleObjects;

	for (auto& e : m_mapPlayer) {
		Player* pPlayer = e.second;

		for (auto& unit : pPlayer->getObjects()) {
			if (pViewPort->isObjectVisible(unit)) {
				// ToDo: Fuer Gebaeude korrekt implementieren (fuer vector<Pos> abfragem
				if (!gDebugManager.m_map_considerExploration || mapExplored[unit->getTileLocation().y][unit->getTileLocation().x]) {
					vecVisibleObjects.push_back(unit);

					//   Point posLocal_tile = pViewPort->worldToScreen_tile(unit->getPos());
					//   pRenderer->renderTexture(unit->texture(), pViewPort->tileToPixel(posLocal_tile));
				}
			}

			if (gDebugManager.m_object_drawVisibilityRanges && (pPlayer == pHumanPlayer)) {
				std::vector<Tile*> vecVisibleTiles;
				getVisibleTiles(unit, unit->getSight(), vecVisibleTiles);

				for (auto& pTile : vecVisibleTiles) {
					Rect r;

					// funktionen zusammenfassen??
					r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
					r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
					r.w = Tile::TileWidth_px;
					r.h = Tile::TileWidth_px;

					pRenderer->drawRect(r, 0, 0, 255);
				}

				IArmed* pArmedUnit = dynamic_cast<IArmed*>(unit);
				if (pArmedUnit) {
					std::vector<Tile*> vecTilesInAttackRange;
					getVisibleTiles(unit, pArmedUnit->attackRange(), vecTilesInAttackRange);

					for (auto& pTile : vecTilesInAttackRange) {
						Rect r;

						// funktionen zusammenfassen??
						r.x = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).x;
						r.y = pViewPort->tileToPixel(pViewPort->worldToScreen_tile(pTile->getPos())).y;
						r.w = Tile::TileWidth_px;
						r.h = Tile::TileWidth_px;

						pRenderer->drawRect(r, 255, 0, 0);
					}
				}
			}
		}
	}




	// First vanishing objects so that other land units can stand on them (=are rendered on top of them)
	for (auto& pUnit : vecVisibleObjects) {
		/*
		if (pUnit->isVanishing()) {
			Point posLocal_tile = pViewPort->worldToScreen_tile(pUnit->getTileLocation());
			pRenderer->renderSprite(pViewPort->tileToPixel(posLocal_tile), pUnit->sprite());
		}*/
	}

	// Second buildings
	for (auto& pUnit : vecVisibleObjects) {
		Building* pBuilding = dynamic_cast<Building*>(pUnit);
		if (pBuilding != nullptr) {
			Point posLocal_tile = pViewPort->worldToScreen_tile(pUnit->getTileLocation());
			Point posLocal_px = pViewPort->tileToPixel(posLocal_tile);

			if (pBuilding->isUnderConstruction()) {
				/*
				 The construction site patches (small and big one) have a width and height of 64 pixels.
				 This matches perfectly to small 2x2 buildings such as the farm.
				 For bigger buildings like townhall or barracks we must calculate an offset such that the construction
				 site patches are placed in the middle of the construction site and not at the top left position.
				 */
				if (pBuilding->getBuildingState() == Building::BuildingState::InConstruction1 || pBuilding->getBuildingState() == Building::BuildingState::InConstruction2) {
					int tile_size = gMapUnitTilesize[typeid(*pBuilding)];

					tile_size -= 2; // For 2x2 this will be zero, so no offset calculation takes place

					posLocal_px.x += (int)(Tile::TileWidth_px * (double)tile_size / 2);
					posLocal_px.y += (int)(Tile::TileWidth_px * (double)tile_size / 2);
				}

			}

			pRenderer->renderSprite(posLocal_px, pUnit->sprite());
		}
	}


	// Third land units (catapults, death knights etc. may cover building parts a bit)
	for (auto& pUnit : vecVisibleObjects) {
		if (dynamic_cast<Building*>(pUnit) != nullptr)
			continue;

		// Todo: buildings are included? does land match?
		if (pUnit->getType() == Object::Type::Land || pUnit->getType() == Object::Type::Sea) {
			Point posLocal_tile = pViewPort->worldToScreen_tile(pUnit->getTileLocation());
			Point posLocal_px = pViewPort->tileToPixel(posLocal_tile);

			// Applies when a unit is moving between two tiles
			posLocal_px.x += pUnit->getTilePositionOffset().x;
			posLocal_px.y += pUnit->getTilePositionOffset().y;

			pRenderer->renderSprite(posLocal_px, pUnit->sprite());

			// Draw buffs
			Unit* pUnit1 = dynamic_cast<Unit*>(pUnit);
			if (pUnit1->hasActiveBuffs()) {
				auto& activeBuffs = pUnit1->getActiveBuffs();


				for (const auto& [buffType, buff] : activeBuffs) {
					Sprite* pIcon = g_pRessourceManager->m_mapBuffSprites[buffType];
					pRenderer->renderSprite(posLocal_px, pIcon);
					posLocal_px.x += 16;
				}
			}
		}
	}



	// Fourth air units
	for (auto& pUnit : vecVisibleObjects) {
		if (pUnit->getType() == Object::Type::Air) {
			Point posLocal_tile = pViewPort->worldToScreen_tile(pUnit->getTileLocation());
			pRenderer->renderSprite(pViewPort->tileToPixel(posLocal_tile), pUnit->sprite());
		}
	}


	// Fifth: Top level animations, missles, etc.
	for (auto& [pTile, animation] : m_mapAnimations) {
		if (pViewPort->isTileVisible(pTile)) {
			Point posLocal_tile = pViewPort->worldToScreen_tile(Point{ pTile->getX(), pTile->getY() });
			pRenderer->renderSprite(pViewPort->tileToPixel(posLocal_tile), animation.getCurrentFrame());
		}
	}



	std::vector<MapObject*> vecVisibleMapObjects;

	std::copy_if(m_vecMapObjects.begin(), m_vecMapObjects.end(), std::back_inserter(vecVisibleMapObjects),
		[pViewPort](MapObject* o) {
			return pViewPort->isObjectVisible(o);
		});


	for (MapObject* pObject : vecVisibleMapObjects) {
		Point posScreen_px = pViewPort->worldToScreen_px(pObject->getPositionWorld_px());
		pRenderer->renderSprite(posScreen_px, pObject->getSprite());
	}
}

