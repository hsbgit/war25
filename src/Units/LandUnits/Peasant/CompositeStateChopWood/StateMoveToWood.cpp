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
#include "CompositeStateChopWood.h"
#include "Map/Map.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include <iostream>


StateMoveToWood::StateMoveToWood(Peasant* pPeasant, State* pIdle) : StateMoving(pPeasant), m_pIdle(pIdle) {

}


void StateMoveToWood::setCompositeState(CompositeStateChopWood* pComposite) {
    m_pComposite = pComposite;
}


void StateMoveToWood::onEnter(const Event* pEvent) {
    if (pEvent) {
        // If pEvent exists onEnter has been initially called by the composite state
        // Save this position for future cycles
        goldmine_tile_world.x = pEvent->tileX();
        goldmine_tile_world.y = pEvent->tileY();

        if (m_pComposite) {
            m_pComposite->setLastWoodPosition(goldmine_tile_world);
        }
    } else {
        // No specific target - try to use last known wood position first
        std::cerr << "DEBUG: onEnter called with no event (after delivery)" << std::endl;
        if (m_pComposite) {
            Point lastPos = m_pComposite->getLastWoodPosition();
            std::cerr << "DEBUG: Last saved position is (" << lastPos.x << "," << lastPos.y << ")" << std::endl;
            if (lastPos.x != -1 && lastPos.y != -1) {
                // Check if there's still wood at the last position
                Tile* lastWoodTile = m_pOwner->getMap()->getTile(lastPos);
                if (lastWoodTile && lastWoodTile->isWood()) {
                    goldmine_tile_world = lastPos;
                    std::cerr << "DEBUG: Returning to last wood position (" << lastPos.x << "," << lastPos.y << ")" << std::endl;
                } else {
                    std::cerr << "DEBUG: Last wood position no longer has wood, searching for new wood" << std::endl;
                    // Last position no longer has wood, search for new wood
                    Tile* peasantTile = m_pOwner->getTile();
                    Tile* woodTile = m_pOwner->getMap()->getNeighbourTileWithWood(peasantTile, 3);

                    if (woodTile) {
                        goldmine_tile_world = woodTile->getPos();
                        m_pComposite->setLastWoodPosition(goldmine_tile_world);
                    } else {
                        // No wood found in reasonable distance
                        m_pNextReachableTile = nullptr;
                        return;
                    }
                }
            } else {
                // No previous position, search for wood in the area
                std::cerr << "DEBUG: No previous position, searching for wood in area" << std::endl;
                Tile* peasantTile = m_pOwner->getTile();
                Tile* woodTile = m_pOwner->getMap()->getNeighbourTileWithWood(peasantTile, 3);

                if (woodTile) {
                    goldmine_tile_world = woodTile->getPos();
                    m_pComposite->setLastWoodPosition(goldmine_tile_world);
                    std::cerr << "DEBUG: Found new wood at (" << goldmine_tile_world.x << "," << goldmine_tile_world.y << ")" << std::endl;
                } else {
                    std::cerr << "DEBUG: No wood found in reasonable distance" << std::endl;
                    // No wood found in reasonable distance
                    m_pNextReachableTile = nullptr;
                    return;
                }
            }
        } else {
            // Fallback: search for wood in the area (no composite reference)
            std::cerr << "DEBUG: No composite reference, using fallback search" << std::endl;
            Tile* peasantTile = m_pOwner->getTile();
            Tile* woodTile = m_pOwner->getMap()->getNeighbourTileWithWood(peasantTile, 3);

            if (woodTile) {
                goldmine_tile_world = woodTile->getPos();
                std::cerr << "DEBUG: Fallback found wood at (" << goldmine_tile_world.x << "," << goldmine_tile_world.y << ")" << std::endl;
            } else {
                std::cerr << "DEBUG: Fallback found no wood" << std::endl;
                // No wood found in reasonable distance
                m_pNextReachableTile = nullptr;
                return;
            }
        }
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
