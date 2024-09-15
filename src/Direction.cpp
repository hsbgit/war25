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
#include "Direction.h"
#include "Map/Tile.h"
#include <map>
#include <cassert>


bool operator<(const Point& p1, const Point& p2)
{
    return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
}


bool operator==(const Point& p1, const Point& p2)
{
    return (p1.x == p2.x) && (p1.y == p2.y);
}


// Relative cardinal direction of two coordinates
std::map<Point, Direction> mapCardinalDirectionLookup{
    { {-1,-1}, Direction::NW},
    { { 0,-1}, Direction::N},
    { { 1,-1}, Direction::NE},

    { {-1, 0}, Direction::W},
    // 0,0 would be the current location
    { { 1, 0}, Direction::E},

    { {-1, 1}, Direction::SW},
    { { 0, 1}, Direction::S},
    { { 1, 1}, Direction::SE}
};


Direction getTargetCardinalDirection(const Point& currPos_tile, const Point& targetPos_tile) {
    Point s = { sgn(targetPos_tile.x - currPos_tile.x), sgn(targetPos_tile.y - currPos_tile.y) };
    assert(mapCardinalDirectionLookup.contains(s));
    return mapCardinalDirectionLookup[s];
}


Direction getTargetCardinalDirection(const Tile* pCurrTile, const Tile* pTargetTile) {
    Point s = { sgn(pTargetTile->getX() - pCurrTile->getX()), sgn(pTargetTile->getY() - pCurrTile->getY()) };
    assert(mapCardinalDirectionLookup.contains(s));
    return mapCardinalDirectionLookup[s];
}
