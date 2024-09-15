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
#include <cassert>


StateEnterBuildingForDelivery::StateEnterBuildingForDelivery(Peasant* pPeasant) : State(pPeasant) {

}


void StateEnterBuildingForDelivery::onEnter(const Event* pEvent) {
    Tile* pTile = m_pOwner->getTile();

    // Todo: Check whether townhall/saw mill/whatever actually belongs to this player, not to any other player
    m_pResourceAcceptor = m_pOwner->getMap()->isNextToBuilding<IResourceAcceptor>(pTile);

    if (m_pResourceAcceptor) {
        m_pOwner->getMap()->releaseObjectFromTile(m_pOwner);
        m_deliveryTicks = calculateRequiredTicksForGameAction(2 * 1000);
    }
}


State* StateEnterBuildingForDelivery::process() {
    if (!m_pResourceAcceptor)
        return transitionToNextState();

    if (m_deliveryTicks > 0) {
        m_deliveryTicks--;
    }

    // Todo: What if townhall gets destroyed?
    // Todo: m_pResourceAcceptor as tile not as pointer

    if (m_deliveryTicks-- == 0) {
        Tile* pFreeTileNextToResourceAcceptor = m_pOwner->getMap()->getFreeTileAroundBuilding(dynamic_cast<Building*>(m_pResourceAcceptor), m_pOwner);

        if (!pFreeTileNextToResourceAcceptor)
            return this;

        assert(pFreeTileNextToResourceAcceptor->getSeaOrLandUnit() == nullptr);
        m_pOwner->getMap()->placeUnit(pFreeTileNextToResourceAcceptor, m_pOwner);


        // ToDo: this is ugly. make it nice
        const Resources& rBasic = m_pOwner->getOwner()->getBasicResourceProduction();
        const Resources& rAdditional = m_pOwner->getOwner()->getAdditionResourceProduction();

        Resources minedGold = Resources(rBasic.gold() + rAdditional.gold());

        m_pOwner->getOwner()->addToResources(minedGold);
        dynamic_cast<Peasant*>(m_pOwner)->setCarriedResource(ResourceType::Nothing);
        return transitionToNextState();
    }

    return this;
}
