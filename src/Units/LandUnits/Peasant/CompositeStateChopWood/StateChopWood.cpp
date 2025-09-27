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
#include "StateChopWood.h"
#include "Global/PseudoRandom.h"
#include "Global/GlobalAttributes.h"
#include "Resources/SoundManager.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Map/Map.h"
#include <algorithm>

// Static member definition
std::vector<StateLumberWood*> StateLumberWood::s_activeInstances;


StateLumberWood::StateLumberWood(Peasant* pPeasant, State* pIdle) : State(pPeasant), m_pIdle(pIdle) {
    m_ticksPerSecond = calculateRequiredTicksForGameAction(1 * 1000);
}


void StateLumberWood::setMoveToWoodState(State* pMoveToWood) {
    m_pMoveToWood = pMoveToWood;
}


void StateLumberWood::onEnter(const Event* pEvent) {
    m_requiredTicksForChop = calculateRequiredTicksForGameAction(5 * 1000); // 25
    m_pTileWithWood = identifyCloseByWood();

    // Register for debug visualization
    s_activeInstances.push_back(this);
}


void StateLumberWood::onExit() {
    deregisterFromDebug();
    State::onExit(); // Call base class implementation
}


State* StateLumberWood::process() {
    if (!m_pTileWithWood) {
        // No wood on a direct neighbour tile?
        // Go back to move state to search for wood in larger area
        if (m_pMoveToWood) {
            return m_pMoveToWood;
        }
        return m_pIdle;
    }

    if (!m_pTileWithWood->isWood()) {
        // Another Peasant chopped faster ;( 
        // We have wasted our time and have to start over...
        Tile* pNewTileWithWood = identifyCloseByWood();

        if (!pNewTileWithWood) {
            // Go back to move state to search for wood in larger area
            if (m_pMoveToWood) {
                return m_pMoveToWood;
            }
            return m_pIdle;
        }

        m_requiredTicksForChop = calculateRequiredTicksForGameAction(5 * 1000);
        m_pTileWithWood = pNewTileWithWood;
        //     g_pEventBroker.addEvent(Event(m_pOwner->geti, (EventID)PeasantActions::Harvest, m_pTileWithWood->getX(), m_pTileWithWood->getY()));
    }

    if (m_requiredTicksForChop-- % m_ticksPerSecond == 0) {
        // ToDo: only loud/hearable when in field of view. this has currently 100% everywhere
        g_pSoundManager->playWAV("misc/tree_chopping/" + std::to_string(PseudoRandom::rnd(1, 4)) + ".wav");
    }

    if (m_requiredTicksForChop == 0) {
        m_pTileWithWood->destroy();
        dynamic_cast<Peasant*>(m_pOwner)->setCarriedResource(ResourceType::Wood);
        return transitionToNextState();
    }

    return this;
}


Tile* StateLumberWood::identifyCloseByWood() {
    Tile* posPeasant = m_pOwner->getTile();

    // First check immediate neighbors - peasants should only chop wood directly adjacent to them
    Tile* pTileWithWood = m_pOwner->getMap()->getNeighbourTileWithWood(posPeasant, 1);

    // If no wood found adjacent, we need to move to wood first
    // Return nullptr so the peasant goes back to idle and gets reassigned
    return pTileWithWood;
}


void StateLumberWood::deregisterFromDebug() {
    auto it = std::find(s_activeInstances.begin(), s_activeInstances.end(), this);
    if (it != s_activeInstances.end()) {
        s_activeInstances.erase(it);
    }
}


std::vector<Tile*> StateLumberWood::getCurrentSearchArea() const {
    std::vector<Tile*> searchArea;

    if (!m_pOwner) {
        return searchArea;
    }

    // Check if the peasant is still placed on the map before accessing its tile
    if (!m_pOwner->isPlacedOnMap()) {
        return searchArea;
    }

    Tile* peasantTile = m_pOwner->getTile();
    if (!peasantTile) {
        return searchArea;
    }

    Map* pMap = m_pOwner->getMap();
    if (!pMap) {
        return searchArea;
    }

    // Get all tiles within search radius (up to 3 tiles away for visualization)
    // This shows where the peasant would search for wood to move to
    int peasantX = peasantTile->getX();
    int peasantY = peasantTile->getY();

    for (int radius = 1; radius <= 3; radius++) {
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dy = -radius; dy <= radius; dy++) {
                // Skip tiles that are not exactly at this radius
                if (std::abs(dx) != radius && std::abs(dy) != radius) {
                    continue;
                }

                int x = peasantX + dx;
                int y = peasantY + dy;

                Tile* tile = pMap->getTile({x, y});
                if (tile) {
                    searchArea.push_back(tile);
                }
            }
        }
    }

    return searchArea;
}


void StateLumberWood::getActiveWoodSearchAreas(std::vector<std::vector<Tile*>>& searchAreas) {
    searchAreas.clear();

    // Clean up only truly invalid instances (null pointers or dead peasants)
    auto it = s_activeInstances.begin();
    while (it != s_activeInstances.end()) {
        StateLumberWood* instance = *it;
        if (!instance || !instance->m_pOwner || instance->m_pOwner->isDead()) {
            it = s_activeInstances.erase(it);
        } else {
            ++it;
        }
    }

    // Now collect search areas from valid instances that are currently on the map
    for (StateLumberWood* instance : s_activeInstances) {
        // Only show debug visualization for peasants currently on the map
        // (not when they're inside buildings delivering resources)
        if (instance->m_pOwner->isPlacedOnMap()) {
            std::vector<Tile*> instanceSearchArea = instance->getCurrentSearchArea();
            if (!instanceSearchArea.empty()) {
                searchAreas.push_back(instanceSearchArea);
            }
        }
    }
}
