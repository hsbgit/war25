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
#include <SDL.h>

class Tile;

// boost true sign https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

bool operator<(const SDL_Point& p1, const SDL_Point& p2);
bool operator==(const SDL_Point& p1, const SDL_Point& p2);

/*
 * Do not change the direction ordering as their integer value is important!
 * Warcraft sprites inside the *.png files are ordered from left to right as N, NE, E, SE, S. 
 * SW, W and NW can be created by flipping existing *.png
 * See, e.g., ResourceManager_wartools::getCritterSprite()
 */
enum class Direction { N = 0, NE, E, SE, S, SW, W, NW};

Direction getTargetCardinalDirection(const SDL_Point& currPos_tile, const SDL_Point& targetPos_tile);
Direction getTargetCardinalDirection(const Tile* pCurrTile, const Tile* pTargetTile);
