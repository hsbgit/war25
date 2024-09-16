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

#include "State.h"
#include "Map/Path.h"
#include "Direction.h"
#include "Types.h"

class Tile;


class StateMoving : public State {
public:
    StateMoving(Object* pOwner);

    void onEnter(const Event* pEvent) override;
    void onExit() override;

    State* process() override;

    void setTargetTile(const Point& targetPos_world);


private:
    Tile* isTargetTileANeighbourTile();
    Tile* m_pNextTileToMove = nullptr;
    Tile* getNextTile(bool* pathTemporarilyBlocked);
    Tile* getNextPathTile(bool* pathTemporarilyBlocked);

    int m_ticksPerStep;
    int m_tickCounter = 0;


    void setDirection(Direction direction);
    bool m_directionChanged = false; // True if the direction changed, e.g., from N to NW. This information is used to load the appropriate Animation
    Direction m_currentDirection;

    Path m_path; // Only used if the unit is blocked / the straight way is not working

    Point m_tilePositionOffset_px;

    Point m_targetPos_world;
    Tile* m_pTargetTile = nullptr;
};
