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
#include "Tile.h"
#include "Map/Map.h"
#include "Global/Object.h"
#include "Resources/resourcemanager_wartools.h"
#include "Sprite.h"
#include <algorithm>
#include <cassert>
#include <iostream>


Map* Tile::m_pMap = nullptr;
const int Tile::TileWidth_px = 32;


Tile::Tile() {

}


Tile::Tile(int x, int y, std::uint16_t tile_id) {
	m_pos_tile.x = x;
	m_pos_tile.y = y;

	setSprite(tile_id);


	// If you want to know more about the used ids scroll down to lumber

	// Blue water (ships)
	if (tile_id >= 0x01a0 && tile_id <= 0x01d1) {
		m_isWater = true;
	}

	// Dark water
	if (tile_id >= 0x0010 && tile_id <= 0x0023) {
		m_isWater = true;
	}

	// Dark water
	if (tile_id >= 0x0100 && tile_id <= 0x0192) {
		m_isWater = true;
	}

	// Coast tiles
	if (tile_id >= 0x02a0 && tile_id <= 0x02d1) {
		m_isCoast = true;
	}

	// Coast tiles
	if (tile_id >= 0x200 && tile_id <= 0x0292) {
		m_isCoast = true;
		// Not water and also not walkable
	}

	// Dirt
	if (tile_id >= 0x003a && tile_id <= 0x004b) {
		m_isLand = true;
	}

	// Dirt
	if (tile_id >= 0x0030 && tile_id <= 0x0049) {
		m_isLand = true;
	}

	// Dirt
	if (tile_id >= 0x0300 && tile_id <= 0x0392) {
		m_isLand = true;
	}



	// Mountains
	if (tile_id >= 0x04a0 && tile_id <= 0x04d0) {
		m_isRocks = true;
		m_isLand = true;
	}

	// Mountains
	if (tile_id >= 0x0080 && tile_id <= 0x0083) {
		m_isRocks = true;
		m_isLand = true;
	}

	// Mountains
	if (tile_id >= 0x0400 && tile_id <= 0x0491) {
		m_isRocks = true;
		m_isLand = true;
	}


	// Grass
	if (tile_id >= 0x005a && tile_id <= 0x006f) {
		m_isLand = true;
	}

	// Grass
	if (tile_id >= 0x05a1 && tile_id <= 0x06d1) {
		m_isLand = true;
	}

	// Grass
	if (tile_id >= 0x0050 && tile_id <= 0x0069) {
		m_isLand = true;
	}


	// Grass
	if (tile_id >= 0x0500 && tile_id <= 0x0692) {
		m_isLand = true;
	}


	// Lumber
	if (tile_id >= 0x07a0 && tile_id <= 0x07d1) {
		m_isLand = true;
		m_isWood = true;
	}

	// Lumber
	// Where do all of these numbers from? Open summer.lua and search for "070" which means 0x70
	// There is the following entry
	//      "solid", { "forest", "land", "forest", "unpassable",
	//      { 125, 127, 128}}, -- 070
	// So we have wood starting at 0x70 and three tiles in total -> 0x70 to x72
	if (tile_id >= 0x0070 && tile_id <= 0x0072) {
		m_isLand = true;
		m_isWood = true;
	}

	// Lumber
	if (tile_id >= 0x0700 && tile_id <= 0x0791) {  // 0x730
		m_isLand = true;
		m_isWood = true;
	}



	// Walls
	if (tile_id >= 0x090 && tile_id <= 0x00c4) {
		m_isLand = true;
		m_isWall = true;
	}

	// Walls
	if (tile_id >= 0x0800 && tile_id <= 0x09d4) {
		m_isLand = true;
		m_isWall = true;
	}

	m_tile_id = tile_id;
}


Tile::~Tile() {
	/*
	 * Note: Since 'm_vecNeighbourTiles_SameType' is a subset of 'm_vecNeighbourTiles' (contains
	 * same objects) we only have to delete the 'm_vecNeighbourTiles' vector.
	 */
	for (auto& pNeighbour : neighbourTiles) {
		delete pNeighbour.second; /// !!!
	}
}


void Tile::setSprite(int tile_id) {
	m_tileSprite = g_pRessourceManager->getTileSprite(tile_id);
}


void Tile::destroy() {
	// 0x0056 grass
	// rock 0x0804
	if (m_isRocks) {
		m_isRocks = false;
		setSprite(ResourceManager_wartools::RemovedRock);
	}
	else if (m_isWood) {
		m_isWood = false;
		setSprite(ResourceManager_wartools::RemovedWood);
	}
	else if (m_isWall) {
		m_isWall = false;
		setSprite(ResourceManager_wartools::DestroyedWalls);
	}
}


// airPath: Shorter ways for air units as all surrounding tiles can be used, if there is no other AirUnit
NeighbourNodes Tile::getFreeNeighbourNodes(int unitSize_tiles, bool airPath) const {
	NeighbourNodes freeNodes = airPath ? neighbourTiles : neighbourTiles_SameType;

	// We have to cases: Land/Sea and Air.
	// For both cases we have to check whether there is already another unit of the same type on a neighbour tile. E.g. another air unit
	// As ships and air units have a size of 2x2 we cannot simply check for "pTile->getAirUnit() != nullptr" as the Air unit of the neighbour tile
	// might be the same unit. This is why we have to additionally compare the actual pointers before erasing
	std::erase_if(freeNodes, [this, &airPath](const auto& pair) {
		const Tile* pTile = (Tile*)pair.second;

		if (airPath && pTile->getAirUnit() && (pTile->getAirUnit() != this->getAirUnit()))
			return true;
		else if (!airPath && (pTile->m_isRocks || pTile->m_isWood || pTile->m_isWall || (pTile->getSeaOrLandUnit() && (pTile->getSeaOrLandUnit() != this->getSeaOrLandUnit()))))
			return true;

		return false;
		});


	if (unitSize_tiles > 1) {
		// We have to make additional checks for the remaining tiles as the unit size is 2x2
		Object* pObjectToCheck = airPath ? getAirUnit() : getSeaOrLandUnit();

		std::erase_if(freeNodes, [this, &airPath, pObjectToCheck](const auto& pair) {
			const Tile* pTile = (Tile*)pair.second;
			return !m_pMap->canBePlacedAtPosition(pObjectToCheck, pTile->getX(), pTile->getY());   // Crash bei AStar wenn der Pfad in der Zukunft liegt
			});
	}


	/*
	// ToDo: Size can just be 1 (all land units) or 2 (ships and air units)
	if (unitSize_tiles > 1) {
		// Reference point for a 2x2 unit is always the upper left tile

		// Ordering is important: Check for diagonal movement first as Directions N and W may be removed in the next two if statements
		if (freeNodes.contains(Direction::NW) && (!freeNodes.contains(Direction::N) || !freeNodes.contains(Direction::W))) {
			freeNodes.erase(Direction::NW);
		}


		if (freeNodes.contains(Direction::NE) && (!freeNodes.contains(Direction::N) || !freeNodes.contains(Direction::W))) {
			freeNodes.erase(Direction::NW);
		}

		// To be able to move northwards, the N and NE tiles must be free
		// Moving North is just possibly if
		if (freeNodes.contains(Direction::N) && !freeNodes.contains(Direction::NE)) {
			freeNodes.erase(Direction::N);
		}

		if (freeNodes.contains(Direction::W) && !freeNodes.contains(Direction::SW)) {
			freeNodes.erase(Direction::W);
		}

		freeNodes[Direction::E] = neighbourTiles.at(Direction::E);
		freeNodes[Direction::SE] = neighbourTiles.at(Direction::SE);
		freeNodes[Direction::S] = neighbourTiles.at(Direction::S);
	}
	*/
	return freeNodes;
}

// permanently blocked -> wall, new building
// temporarilyBlocked -> unit which is moving
bool Tile::isBlockedForUnit(const Object* pObj, Object** pBlockingObject) const {
	if (pObj->isAirObject()) {
		if (pBlockingObject)
			*pBlockingObject = m_pAirUnit;
		return (m_pAirUnit && (m_pAirUnit != pObj)) ? true : false;
	}

	if (m_isCoast) {
		if (pBlockingObject)
			*pBlockingObject = m_pUnit; // pBlockingObject may be a nullptr if m_pUnit is a nullptr, which is completely fine
		return true;
	}

	assert(pObj->isLandObject() || pObj->isSeaObject());

	// Is already another land unit or sea unit associated to this tile?
	if (pObj->isLandObject() && isLand()) {
		if (m_isRocks || m_isWood || m_isWall || (m_pUnit && (m_pUnit != pObj))) {
			if (pBlockingObject)
				*pBlockingObject = m_pUnit;
			return true;
		}
	}
	else if (pObj->isSeaObject() && isSea() && m_pUnit && (m_pUnit != pObj)) {
		if (pBlockingObject)
			*pBlockingObject = m_pUnit;
		return true;
	}

	return false;
}


void Tile::assign(Object* pObj) {
	assert(!m_pUnit || !m_pAirUnit); // ToDo: Assert durch exception

	if (pObj->isAirObject()) {
		m_pAirUnit = pObj;
	}
	else {
		m_pUnit = pObj; // Building or LandUnit
	}
}


void Tile::release(Object* pObj) {
	assert((pObj == m_pUnit) || (pObj == m_pAirUnit));

	if (pObj == m_pUnit) {
		m_pUnit = nullptr;
	}
	else if (pObj == m_pAirUnit) {
		m_pAirUnit = nullptr;
	}
}
