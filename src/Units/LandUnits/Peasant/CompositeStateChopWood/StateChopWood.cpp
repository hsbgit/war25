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


StateLumberWood::StateLumberWood(Peasant* pPeasant, State* pIdle) : State(pPeasant), m_pIdle(pIdle) {
    m_ticksPerSecond = calculateRequiredTicksForGameAction(1 * 1000);
}


void StateLumberWood::onEnter(const Event* pEvent) {
    m_requiredTicksForChop = calculateRequiredTicksForGameAction(5 * 1000); // 25
    m_pTileWithWood = identifyCloseByWood();
}


State* StateLumberWood::process() {
    if (!m_pTileWithWood) {
        // No wood on a direct neighbour tile?
        // ToDo: switch back to "go to resource"
        return m_pIdle;
    }

    if (!m_pTileWithWood->isWood()) {
        // Another Peasant chopped faster ;( 
        // We have wasted our time and have to start over...
        Tile* pNewTileWithWood = identifyCloseByWood();

        if (!pNewTileWithWood) {
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


    Tile* pTileWithWood = m_pOwner->getMap()->getNeighbourTileWithWood(posPeasant);

    if (pTileWithWood) {
        // ToDo: Look in correct direction
    }

    return pTileWithWood;
}
