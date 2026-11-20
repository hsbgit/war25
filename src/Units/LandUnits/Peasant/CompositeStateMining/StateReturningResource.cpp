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
#include "Buildings/Building.h"
#include "Global/GlobalAttributes.h"
#include "StateEnterBuildingForDelivery.h"
#include <climits>


// Todo: more generalization -> oil tanker?
StateReturningResource::StateReturningResource(Unit* pCollector, State* pIdle) : StateMoving(pCollector), m_pIdle(pIdle) {

}


void StateReturningResource::onEnter(const Event* pEvent) {
	selectNearestAcceptor();
	m_ticksUntilAcceptorCheck = calculateRequiredTicksForGameAction(2 * 1000);
}


void StateReturningResource::selectNearestAcceptor() {
    m_pTileResourceAcceptor = nullptr;
    m_pApproachTile = nullptr;
    std::vector<Object*> vecObjects = m_pOwner->getOwner()->getObjects();

    Tile* pTileNearestResourceAcceptor = nullptr;
    int minDistance = INT_MAX;

    for (Object* pObj : vecObjects) {
        IResourceAcceptor* pRA = dynamic_cast<IResourceAcceptor*>(pObj);


        // Todo: there was a bug when pObj was not connected to a tile, meaning pObj->getTile()->getX() caused a crash
        if (pRA && !pObj->isDead() && pObj->getTile() && m_pOwner->getTile() &&
            pRA->acceptsResource(dynamic_cast<IResourceCollector*>(m_pOwner)->currentlyCarrying())) {
            int distance = std::abs(m_pOwner->getTile()->getX() - pObj->getTile()->getX()) + std::abs(m_pOwner->getTile()->getY() - pObj->getTile()->getY());

            if (distance < minDistance) {
                minDistance = distance;
                pTileNearestResourceAcceptor = pObj->getTile();
            }
        }
    }

    if (pTileNearestResourceAcceptor) {
        m_pTileResourceAcceptor = pTileNearestResourceAcceptor;
        auto* pBuilding = dynamic_cast<Building*>(pTileNearestResourceAcceptor->getSeaOrLandUnit());
        m_pApproachTile = pBuilding
            ? m_pOwner->getMap()->getFreeTileAroundBuilding(pBuilding, m_pOwner) : nullptr;

        if (m_pApproachTile) {
            setTargetTile(m_pApproachTile->getPos());
        }
    }
}


State* StateReturningResource::process() {
	if (--m_ticksUntilAcceptorCheck <= 0) {
		Tile* previous = m_pTileResourceAcceptor;
		selectNearestAcceptor();
		m_ticksUntilAcceptorCheck = calculateRequiredTicksForGameAction(2 * 1000);
		if (!m_pTileResourceAcceptor)
			m_pTileResourceAcceptor = previous;
	}
    auto* pBuilding = m_pTileResourceAcceptor
        ? dynamic_cast<Building*>(m_pTileResourceAcceptor->getSeaOrLandUnit()) : nullptr;
    auto* pAcceptor = dynamic_cast<IResourceAcceptor*>(pBuilding);
    if (!pBuilding || !pAcceptor || pBuilding->isDead() || pBuilding->getOwner() != m_pOwner->getOwner())
        return m_pIdle;

    bool nextToChosenAcceptor = false;
    if (m_pOwner->getTile()) {
        const Point worker = m_pOwner->getTile()->getPos();
        for (Tile* occupied : pBuilding->getOccupiedTiles()) {
            if (std::abs(worker.x - occupied->getX()) <= 1 &&
                std::abs(worker.y - occupied->getY()) <= 1) {
                nextToChosenAcceptor = true;
                break;
            }
        }
    }
    if (nextToChosenAcceptor) {
        if (m_pDeliveryState)
            m_pDeliveryState->setResourceAcceptorTile(m_pTileResourceAcceptor);
        return transitionToNextState();
    }

    if (!m_pApproachTile ||
        (m_pApproachTile->getSeaOrLandUnit() && m_pApproachTile->getSeaOrLandUnit() != m_pOwner)) {
        m_pApproachTile = m_pOwner->getMap()->getFreeTileAroundBuilding(pBuilding, m_pOwner);
        if (!m_pApproachTile)
            return this;
        setTargetTile(m_pApproachTile->getPos());
    }

    return StateMoving::process();
}
