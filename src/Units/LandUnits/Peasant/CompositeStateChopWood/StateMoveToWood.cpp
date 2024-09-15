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
#include "StateMoveToWood.h"
#include "Map/Map.h"
#include "Units/LandUnits/Peasant/Peasant.h"


StateMoveToWood::StateMoveToWood(Peasant* pPeasant, State* pIdle) : StateMoving(pPeasant), m_pIdle(pIdle) {

}


void StateMoveToWood::onEnter(const Event* pEvent) {
    if (pEvent) {
        // If pEvent exists onEnter has been initially called by the composite state
        // If pEvent is a nullptr, the peasant brought back gold into the townhall and now wants to go to the mine again
        goldmine_tile_world.x = pEvent->tileX();
        goldmine_tile_world.y = pEvent->tileY();
    }


    // Typically pEvent will point to a tile with lumber on it as the user right clicked on such a tile which caused the lumbering event
    // However, AStar will not find a valid path as the target tile is blocked by lumber. Thus, we try to find a free tile nearby
    m_pNextReachableTile = m_pOwner->getMap()->getNextFreeTile(goldmine_tile_world);


    // We can't start AStar with a destiny tile that is blocked
    if (m_pNextReachableTile) {
        setTargetTile(m_pNextReachableTile->getPos());
    }
}


State* StateMoveToWood::process() {
    if (!m_pNextReachableTile)
        return m_pIdle;

    return StateMoving::process();
}
