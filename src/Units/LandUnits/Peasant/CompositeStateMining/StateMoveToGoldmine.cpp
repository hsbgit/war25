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
#include "StateMoveToGoldmine.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Global/Object.h"
#include "Map/Map.h"
#include "Buildings/Goldmine.h"


StateMoveToGoldmine::StateMoveToGoldmine(Peasant* pPeasant, State* pIdle) : StateMoving(pPeasant), m_pIdle(pIdle) {

}


void StateMoveToGoldmine::onEnter(const Event* pEvent) {
    if (pEvent) {
        // If pEvent exists onEnter has been initially called by the composite state
        // If pEvent is a nullptr, the peasant brought back gold into the townhall and now wants to go to the mine again
        m_initialEvent = Event(pEvent->eventProcessorID(), pEvent->getEventID(), pEvent->tileX(), pEvent->tileY());
    }

    /*
    Let's help AStar a bit here. (CURRENTLY ONLY IMPLEMENTED FOR LAND UNITS!)
    It may be possible that the user clicked on a coastal tile, on wood, on a tile with a unit on it, maybe on a goldmine.
    Thus, the target tile is actually blocked, AStar can not find a path.
    By calling 'getNextFreeTile' we try to figure out if there is any free tile close to the blocked target tile.
    If yes, we use it as the new target tile. If not, we do not move at all
    */
    Point goldmine_tile_world = Point{ (int)m_initialEvent.tileX(), (int)m_initialEvent.tileY() };
    Tile* pTile = m_pOwner->getMap()->getTile(goldmine_tile_world);

    m_pNextReachableTile = nullptr;
    if (pTile->getSeaOrLandUnit() && dynamic_cast<GoldMine*>(pTile->getSeaOrLandUnit())) {
        m_pNextReachableTile = m_pOwner->getMap()->getNextFreeTile(pTile);
    }

    // We can't start AStar with a destiny tile that is blocked
    if (m_pNextReachableTile) {
        setTargetTile(m_pNextReachableTile->getPos());
    }
}


State* StateMoveToGoldmine::process() {
    // Todo: check if townhall still exists or if there is any closer townhall which has been built
    if (!m_pNextReachableTile)
        return m_pIdle;

    return StateMoving::process();
}
