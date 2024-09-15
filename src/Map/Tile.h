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


class Object;
#include <map>
#include <vector>
#include <cassert>

#include "INode.h"
#include "Texture.h"
#include "Types.h"

class Map;
class Sprite;

class Tile : public INode {
public:
	static Map* m_pMap;
	static const int TileWidth_px;

	Tile();
	Tile(int x, int y, std::uint16_t tile_id);
	~Tile();

	// Inherited from INode
	NeighbourNodes getFreeNeighbourNodes(int unitSize_tiles = 1, bool airPath = false) const override;
	inline int getX() const override { return m_pos_tile.x; }
	inline int getY() const override { return m_pos_tile.y; }

	inline const Point& getPos() const {
		return m_pos_tile;
	}

	// The following vectors are set by the Map class.
	// They are used within getFreeNeighbourNodes for AStar pathfinding
	NeighbourNodes neighbourTiles; // Used by air units
	/*
	 * A unit must always be placed on a walkable land or water tile
	 * (air units are not relevant, for them 'm_vecNeighbourTiles' is used to find a path).
	 * Hence the following vector is a subset of 'm_vecNeighbourTiles'.
	 * E.g., if this tile represents water this vector would contain all sourrunding water tiles
	 * which could be used for path finding.
	 */
	NeighbourNodes neighbourTiles_SameType;


	void assign(Object* pObj);
	void release(Object* pObj);
	void destroy();

	void setSprite(int tile_id);



	inline bool hasObject() const {
		return (m_pUnit || m_pAirUnit) ? true : false;
	}

	bool isBlockedForUnit(const Object* pObj, Object** pBlockingObject = nullptr) const;

	inline Object* getSeaOrLandUnit() const {
		return m_pUnit;
	}

	inline Object* getAirUnit() const {
		return m_pAirUnit;
	}

	inline const Sprite& getSprite() const {
		return *m_tileSprite;
	}

	inline bool isSea() const {
		return m_isWater;
	}

	inline bool isLand() const {
		return m_isLand;
	}


	// Returns true if this is a land tile with wood, rocks or a wall on it
	inline bool isDestroyable() const {
		return m_isWood || m_isRocks || m_isWall;
	}

	inline bool isWood() const {
		return m_isWood;
	}

	inline bool isCoast() const {
		return m_isCoast;
	}


	inline std::uint16_t getTileID() const {
		return m_tile_id;
	}

private:
	std::uint16_t m_tile_id;

	/*
	 * 'm_isWater' and 'm_isLand' are only true if an object
	 * can actually be placed on this tile. E.g., coast tiles are not water,
	 * Tiles with rock are not land, etc.
	 */
	bool m_isWater = false;
	bool m_isLand = false;
	bool m_isRocks = false; // Can be destroyed by demolition dwarfes
	bool m_isWood = false;  // Can be chopped or destroyed
	bool m_isWall = false;  // Can be destroyed by demolition dwarfes

	bool m_isCoast = false; // Required when building water buildings since they have to be connected to water and coast tiles. Additionally used for transports

	Object* m_pUnit = nullptr; // Can be LandUnit or SeaUnit
	Object* m_pAirUnit = nullptr;

	Point m_pos_tile;
	Sprite* m_tileSprite = nullptr;
};
