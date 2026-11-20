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
#include "Viewport.h"
#include "Object.h"
#include "GlobalAttributes.h"
#include "Map/MapObject.h"


Viewport gViewport;

Viewport::Viewport() {

}


Viewport::Viewport(int maxMapSideLength_tiles, int windowWidth, int windowHeight) {
    m_viewPos_UL_tile.x = 0;
    m_viewPos_UL_tile.y = 0;

    m_maxMapSideLength_tiles = maxMapSideLength_tiles;
    updateWindowSize(windowWidth, windowHeight);
}


void Viewport::updateWindowSize(int windowWidth, int windowHeight) {
	m_windowWidth_px = windowWidth;
	m_windowHeight_px = windowHeight;
    m_windowWidth_tiles = windowWidth / Tile::TileWidth_px;
    m_windowHeight_tiles = windowHeight / Tile::TileWidth_px;

    if(windowWidth % Tile::TileWidth_px != 0)
        ++m_windowWidth_tiles;

    if(windowHeight % Tile::TileWidth_px != 0)
        ++m_windowHeight_tiles;

    const int cameraX = std::clamp(m_viewPos_UL_tile.x * Tile::TileWidth_px + m_viewOffset_px.x,
        0, std::max(0, m_maxMapSideLength_tiles * Tile::TileWidth_px - m_windowWidth_px));
    const int cameraY = std::clamp(m_viewPos_UL_tile.y * Tile::TileWidth_px + m_viewOffset_px.y,
        0, std::max(0, m_maxMapSideLength_tiles * Tile::TileWidth_px - m_windowHeight_px));
    m_viewPos_UL_tile = {cameraX / Tile::TileWidth_px, cameraY / Tile::TileWidth_px};
    m_viewOffset_px = {cameraX % Tile::TileWidth_px, cameraY % Tile::TileWidth_px};
    updateMaxViewableTiles();
}


void Viewport::setViewPortPosition(const Point& global_tile) {
	const int cameraX = std::clamp(global_tile.x * Tile::TileWidth_px - m_windowWidth_px / 2,
		0, std::max(0, m_maxMapSideLength_tiles * Tile::TileWidth_px - m_windowWidth_px));
	const int cameraY = std::clamp(global_tile.y * Tile::TileWidth_px - m_windowHeight_px / 2,
		0, std::max(0, m_maxMapSideLength_tiles * Tile::TileWidth_px - m_windowHeight_px));
	m_viewPos_UL_tile = {cameraX / Tile::TileWidth_px, cameraY / Tile::TileWidth_px};
	m_viewOffset_px = {cameraX % Tile::TileWidth_px, cameraY % Tile::TileWidth_px};
    updateMaxViewableTiles();
}

void Viewport::panByTiles(int deltaX, int deltaY) {
	panByPixels(deltaX * Tile::TileWidth_px, deltaY * Tile::TileWidth_px);
}

void Viewport::panByPixels(int deltaX, int deltaY) {
	const int cameraX = std::clamp(m_viewPos_UL_tile.x * Tile::TileWidth_px + m_viewOffset_px.x + deltaX,
		0, std::max(0, m_maxMapSideLength_tiles * Tile::TileWidth_px - m_windowWidth_px));
	const int cameraY = std::clamp(m_viewPos_UL_tile.y * Tile::TileWidth_px + m_viewOffset_px.y + deltaY,
		0, std::max(0, m_maxMapSideLength_tiles * Tile::TileWidth_px - m_windowHeight_px));
	m_viewPos_UL_tile = {cameraX / Tile::TileWidth_px, cameraY / Tile::TileWidth_px};
	m_viewOffset_px = {cameraX % Tile::TileWidth_px, cameraY % Tile::TileWidth_px};
	updateMaxViewableTiles();
}


void Viewport::updateMaxViewableTiles() {
    const int visibleWidth = (m_viewOffset_px.x + m_windowWidth_px + Tile::TileWidth_px - 1) / Tile::TileWidth_px;
    const int visibleHeight = (m_viewOffset_px.y + m_windowHeight_px + Tile::TileWidth_px - 1) / Tile::TileWidth_px;
    m_maxViewableMapWidth_tiles = std::min(m_viewPos_UL_tile.x + visibleWidth, m_maxMapSideLength_tiles);
    m_maxViewableMapHeight_tiles = std::min(m_viewPos_UL_tile.y + visibleHeight, m_maxMapSideLength_tiles);
}


bool Viewport::consume(SDL_Event& event) {
    if (event.type != SDL_KEYDOWN)
        return false;

    switch (event.key.keysym.sym) {
        case SDLK_LEFT:
        {
			panByTiles(-1, 0);
            break;
        }

        case SDLK_RIGHT:
        {
			panByTiles(1, 0);
            break;
        }

        case SDLK_UP:
        {
			panByTiles(0, -1);
            break;
        }

        case SDLK_DOWN:
        {
			panByTiles(0, 1);
            break;
        }

        default:
            break;
    }

    return true;
}


bool Viewport::isObjectVisible(const Object* pObj) {
    if(!pObj->isPlacedOnMap())
        return false;

    // '+ (pObj->m_tileSize-1)' since this object might only be partially included within the viewport.
    // Remember: Object position/placement is always the top left tile! E.g., Goldmine:
    // p|T|T
    // T|T|T
    // T|T|T
    // p: Position in Map, Tiles covered by texture belonging to the Goldmine
    const Point& pos_Tile = pObj->getTileTopLeft()->getPos();
    int unitTileSize = gMapUnitTilesize[typeid(*pObj)];

    return isUnitOnTileVisible(pos_Tile, unitTileSize);
}


bool Viewport::isObjectVisible(const MapObject* pObj) {
    const Point& pos_Tile = pixelToTile(pObj->getPositionWorld_px());
    int unitTileSize = 1;

    return isUnitOnTileVisible(pos_Tile, unitTileSize);
}


bool Viewport::isTileVisible(const Tile* pTile) const {
    return isUnitOnTileVisible(Point{ pTile->getX(), pTile->getY() }, 1);

}


bool Viewport::isUnitOnTileVisible(const Point& pos_Tile, int unitTileSize) const {
    if (((pos_Tile.x + unitTileSize) >= m_viewPos_UL_tile.x) && (pos_Tile.x < getMaxViewableMapWidth_tiles())) {
        if ((pos_Tile.y + (unitTileSize) >= m_viewPos_UL_tile.y) && (pos_Tile.y < getMaxViewableMapHeight_tiles())) {
            // ToDo: Units moving from the bottom into the current view area "pop up" -> consider getTilePositionOffset 
            return true;
        }
    }

    return false;
}
