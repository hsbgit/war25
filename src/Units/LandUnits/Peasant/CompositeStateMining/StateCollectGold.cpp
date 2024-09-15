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
#include "StateCollectGold.h"
#include "Buildings/Goldmine.h"
#include "Map/Map.h"
#include "Units/LandUnits/Peasant/Peasant.h"


StateCollectGold::StateCollectGold(Peasant* pPeasant, State* pIdle) : State(pPeasant), m_pIdle(pIdle) {

}


void StateCollectGold::onEnter(const Event* pEvent) {
    Tile* posPeasant = m_pOwner->getTile();
    GoldMine* pGoldMine = m_pOwner->getMap()->isNextToBuilding<GoldMine>(posPeasant);

    if (pGoldMine) {
        m_pTileGoldMine = pGoldMine->getTile();
        m_enteringSuccessful = pGoldMine->enter(dynamic_cast<IResourceCollector*>(m_pOwner));
    }
}


State* StateCollectGold::process() {
    if (!m_pTileGoldMine)
        return m_pIdle;

    GoldMine* pGoldMine = dynamic_cast<GoldMine*>(m_pTileGoldMine->getSeaOrLandUnit());

    if (!m_enteringSuccessful) {
        // There should be a goldmine. If not, it is exhausted and/or destroyed
        if (!pGoldMine)
            return m_pIdle;

        m_enteringSuccessful = pGoldMine->enter(dynamic_cast<IResourceCollector*>(m_pOwner));
    }
    else if (m_enteringSuccessful) {
        // Peasant entered goldmine but the GoldMine disappeared (=exhausted) -> Switch to idle
        if (!pGoldMine)
            return m_pIdle;
    }

    // Transition to StateReturningResource is triggered by 'f_PeasantCarriesGold'
    // This happens when the mining process is done
    return this;
}
