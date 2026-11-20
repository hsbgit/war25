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
	void panByTiles(int deltaX, int deltaY);
	void panByPixels(int deltaX, int deltaY);


	Point getTileCoodinatesAtPosition(const Point& mouseCurrPos) {
		Point coords;
		coords.x = m_viewPos_UL_tile.x + (mouseCurrPos.x + m_viewOffset_px.x) / Tile::TileWidth_px;
		coords.y = m_viewPos_UL_tile.y + (mouseCurrPos.y + m_viewOffset_px.y) / Tile::TileWidth_px;
		return coords;
	}


	Point worldToScreen_px(const Point& global_px) const {
		Point local_px = { (global_px.x - m_viewPos_UL_tile.x * Tile::TileWidth_px - m_viewOffset_px.x),
							   (global_px.y - m_viewPos_UL_tile.y * Tile::TileWidth_px - m_viewOffset_px.y) };
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
	Point pixelToTile(const Point& pos_px) const {
		Point tile = { (pos_px.x + m_viewOffset_px.x) / Tile::TileWidth_px,
						   (pos_px.y + m_viewOffset_px.y) / Tile::TileWidth_px };
		return tile;
	}

	// Converts a tile's world position to world pixel
	Point tileToPixel(const Point& pos_tile) const {
		Point pos_px = { pos_tile.x * Tile::TileWidth_px - m_viewOffset_px.x,
							 pos_tile.y * Tile::TileWidth_px - m_viewOffset_px.y };
		return pos_px;
	}


	inline const Point& getCurrentPosition_TL_tile() const {
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
	Point m_viewOffset_px{0, 0};

	int m_windowHeight_tiles = -1, m_windowWidth_tiles = -1;
	int m_windowWidth_px = 0, m_windowHeight_px = 0;

	int m_maxViewableMapWidth_tiles = -1, m_maxViewableMapHeight_tiles = -1;
	int m_maxMapSideLength_tiles = -1;
};
