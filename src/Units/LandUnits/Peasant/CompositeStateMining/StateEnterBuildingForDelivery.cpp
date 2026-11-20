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
#include "StateEnterBuildingForDelivery.h"
#include "Global/GlobalAttributes.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Player.h"
#include "Buildings/LumberMill.h"
#include <cassert>


StateEnterBuildingForDelivery::StateEnterBuildingForDelivery(Unit* pCollector) : State(pCollector) {

}


void StateEnterBuildingForDelivery::onEnter(const Event* pEvent) {
    m_deliveryTicks = -1;
    Tile* pTile = m_pOwner->getTile();
	if (!pTile)
		return;

    // StateReturningResource chose this exact acceptor. Do not scan again: two
    // adjacent town halls could otherwise make a worker emerge from the wrong one.
	auto* pAcceptorObject = m_pResourceAcceptorTile
		? dynamic_cast<Object*>(m_pResourceAcceptorTile->getSeaOrLandUnit()) : nullptr;
	auto* pResourceAcceptor = dynamic_cast<IResourceAcceptor*>(pAcceptorObject);
    if (pResourceAcceptor && pAcceptorObject &&
		pAcceptorObject->getOwner() == m_pOwner->getOwner() && !pAcceptorObject->isDead()) {
        m_pOwner->getMap()->releaseObjectFromTile(m_pOwner);
        m_deliveryTicks = calculateRequiredTicksForGameAction(2 * 1000);
	}
	else {
		m_pResourceAcceptorTile = nullptr;
	}
}


State* StateEnterBuildingForDelivery::process() {
    if (!m_pResourceAcceptorTile)
        return transitionToNextState();

	auto* pAcceptorBuilding = dynamic_cast<Building*>(m_pResourceAcceptorTile->getSeaOrLandUnit());
	if (!pAcceptorBuilding || pAcceptorBuilding->isDead() ||
		pAcceptorBuilding->getOwner() != m_pOwner->getOwner()) {
		Tile* pFallback = m_pOwner->getMap()->getNextFreeTile(
			m_pResourceAcceptorTile, m_pOwner->getMap()->sideLength());
		if (!pFallback)
			return this;
		m_pOwner->getMap()->placeUnit(pFallback, m_pOwner);
		m_pResourceAcceptorTile = nullptr;
		return transitionToNextState();
	}

    if (m_deliveryTicks > 0)
        --m_deliveryTicks;

    if (m_deliveryTicks == 0) {
        Tile* pFreeTileNextToResourceAcceptor = m_pOwner->getMap()->getFreeTileAroundBuilding(pAcceptorBuilding, m_pOwner);

        if (!pFreeTileNextToResourceAcceptor)
            return this;

        assert(pFreeTileNextToResourceAcceptor->getSeaOrLandUnit() == nullptr);
        m_pOwner->getMap()->placeUnit(pFreeTileNextToResourceAcceptor, m_pOwner);


        // Deliver the correct resource type based on what the peasant is carrying
		auto* pCollector = dynamic_cast<IResourceCollector*>(m_pOwner);
		assert(pCollector);
        ResourceType carriedResource = pCollector->currentlyCarrying();

        if (carriedResource == ResourceType::Gold) {
            const Resources& rBasic = m_pOwner->getOwner()->getBasicResourceProduction();
            const Resources& rAdditional = m_pOwner->getOwner()->getAdditionResourceProduction();

            Resources minedGold = Resources(rBasic.gold() + rAdditional.gold());
            m_pOwner->getOwner()->addToResources(minedGold);
        }
        else if (carriedResource == ResourceType::Wood) {
            const Resources& rBasic = m_pOwner->getOwner()->getBasicResourceProduction();
            const Resources& rAdditional = m_pOwner->getOwner()->getAdditionResourceProduction();

            int woodAmount = rBasic.lumber() + rAdditional.lumber();

            // Check if player has at least one lumber mill for +25 bonus
            bool hasLumberMill = false;
            for (Object* obj : m_pOwner->getOwner()->getObjects()) {
                LumberMill* lumberMill = dynamic_cast<LumberMill*>(obj);
                if (lumberMill && !obj->isDead()) {
                    // Check if building is constructed (for buildings, we can check health > 0)
                    if (obj->getHealthPoints() > 0) {
                        hasLumberMill = true;
                        break;
                    }
                }
            }

            if (hasLumberMill) {
                woodAmount += 25;
            }

            Resources harvestedWood = Resources(0, woodAmount);
            m_pOwner->getOwner()->addToResources(harvestedWood);
        }
		else if (carriedResource == ResourceType::Oil) {
			const Resources& rBasic = m_pOwner->getOwner()->getBasicResourceProduction();
			const Resources& rAdditional = m_pOwner->getOwner()->getAdditionResourceProduction();
			m_pOwner->getOwner()->addToResources(Resources(
				0, 0, rBasic.oil() + rAdditional.oil()));
		}

        pCollector->setCarriedResource(ResourceType::Nothing);
        return transitionToNextState();
    }

    return this;
}
