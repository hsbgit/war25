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

#include <algorithm>
#include "Map/Tile.h"
class Object;
class MapObject;
class Viewport;

extern Viewport gViewport;


class Viewport {
public:
	Viewport();
	Viewport(int maxMapSideLength_tiles, int windowWidth, int windowHeight);
	bool consume(SDL_Event& event);


	void updateWindowSize(int windowWidth, int windowHeight);
	bool isObjectVisible(const Object* pObj);
	bool isObjectVisible(const MapObject* pObj);
	bool isTileVisible(const Tile* pTile) const;

	void setViewPortPosition(const Point& global_tile);


	Point getTileCoodinatesAtPosition(const Point& mouseCurrPos) {
		Point coords;
		coords.x = m_viewPos_UL_tile.x + mouseCurrPos.x / Tile::TileWidth_px;
		coords.y = m_viewPos_UL_tile.y + mouseCurrPos.y / Tile::TileWidth_px;
		return coords;
	}


	Point worldToScreen_px(const Point& global_px) const {
		Point local_px = { (global_px.x - m_viewPos_UL_tile.x * Tile::TileWidth_px),
							   (global_px.y - m_viewPos_UL_tile.y * Tile::TileWidth_px) };
		return local_px;
	}


	Point worldToScreen_tile(const Point& global_tile) const {
		Point local_tile = { (global_tile.x - m_viewPos_UL_tile.x),
							(global_tile.y - m_viewPos_UL_tile.y) };
		return local_tile;
	}

	Point screenToWorld_tile(const Point& local_tile) const {
		Point global_tile = { (local_tile.x + m_viewPos_UL_tile.x),
							(local_tile.y + m_viewPos_UL_tile.y) };
		return global_tile;
	}

	// Converts world pixel coordinates to a tile
	Point pixelToTile(const Point& pos_px) {
		Point tile = { pos_px.x / Tile::TileWidth_px,
						   pos_px.y / Tile::TileWidth_px };
		return tile;
	}

	// Converts a tile's world position to world pixel
	Point tileToPixel(const Point& pos_tile) {
		Point pos_px = { pos_tile.x * Tile::TileWidth_px,
							 pos_tile.y * Tile::TileWidth_px };
		return pos_px;
	}


	inline Point& getCurrentPosition_TL_tile() {
		return m_viewPos_UL_tile;
	}


	int getMaxViewableMapHeight_tiles() const {
		return m_maxViewableMapHeight_tiles;
	}

	int getMaxViewableMapWidth_tiles() const {
		return m_maxViewableMapWidth_tiles;
	}


private:
	void updateMaxViewableTiles();
	bool isUnitOnTileVisible(const Point& pos_Tile, int unitTileSize) const;

	Point m_viewPos_UL_tile;

	int m_windowHeight_tiles = -1, m_windowWidth_tiles = -1;

	int m_maxViewableMapWidth_tiles = -1, m_maxViewableMapHeight_tiles = -1;
	int m_maxMapSideLength_tiles = -1;
};
