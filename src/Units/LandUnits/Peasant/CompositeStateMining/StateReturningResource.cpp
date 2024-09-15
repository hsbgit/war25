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
#include "StateReturningResource.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Global/Object.h"
#include "Player.h"
#include <climits>


// Todo: more generalization -> oil tanker?
StateReturningResource::StateReturningResource(Peasant* pPeasant, State* pIdle) : StateMoving(pPeasant), m_pIdle(pIdle) {

}


void StateReturningResource::onEnter(const Event* pEvent) {
    std::vector<Object*> vecObjects = m_pOwner->getOwner()->getObjects();

    Tile* pTileNearestResourceAcceptor = nullptr;
    int minDistance = INT_MAX;

    for (Object* pObj : vecObjects) {
        IResourceAcceptor* pRA = dynamic_cast<IResourceAcceptor*>(pObj);


        // Todo: there was a bug when pObj was not connected to a tile, meaning pObj->getTile()->getX() caused a crash
        if (pRA && pRA->acceptsResource(dynamic_cast<IResourceCollector*>(m_pOwner)->currentlyCarrying())) {
            int distance = std::abs(m_pOwner->getTile()->getX() - pObj->getTile()->getX()) + std::abs(m_pOwner->getTile()->getY() - pObj->getTile()->getY());

            if (distance < minDistance) {
                minDistance = distance;
                pTileNearestResourceAcceptor = pObj->getTile();
            }
        }
    }

    if (pTileNearestResourceAcceptor) {
        m_pTileResourceAcceptor = m_pOwner->getMap()->getNextFreeTile(pTileNearestResourceAcceptor);

        if (m_pTileResourceAcceptor) {
            setTargetTile(m_pTileResourceAcceptor->getPos());
        }
    }
}


State* StateReturningResource::process() {
    if (!m_pTileResourceAcceptor)
        return m_pIdle;


    // Todo: check if townhall still exists or if there is any closer townhall which has been built

    return StateMoving::process();
}

