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
#include "CompositeStateChopWood.h"
#include "StateMoveToWood.h"
#include "StateChopWood.h"
#include "Units/LandUnits/Peasant/CompositeStateMining/StateReturningResource.h"
#include "Units/LandUnits/Peasant/CompositeStateMining/StateEnterBuildingForDelivery.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include <iostream>


CompositeStateChopWood::CompositeStateChopWood(Peasant* pPeasant) : State(pPeasant) {
    m_movingToRessource = new StateMoveToWood(pPeasant, pPeasant->m_pIdle);
    m_collectRessource = new StateLumberWood(pPeasant, pPeasant->m_pIdle);
    m_returningRessource = new StateReturningResource(pPeasant, pPeasant->m_pIdle);
    m_deliverRessource = new StateEnterBuildingForDelivery(pPeasant);

    // Set the move to wood state reference so StateLumberWood can return to it
    static_cast<StateLumberWood*>(m_collectRessource)->setMoveToWoodState(m_movingToRessource);

    // Set the composite state reference so StateMoveToWood can remember positions
    static_cast<StateMoveToWood*>(m_movingToRessource)->setCompositeState(this);


    // Transitions
    f_PeasantCarriesResource = [this]() {
        IResourceCollector* pPeasant = dynamic_cast<IResourceCollector*>(m_pOwner);
        return (pPeasant->currentlyCarrying() == ResourceType::Wood);
    };


    f_isdead = [this]() {
        return m_pOwner->isDead();
    };

    // todo: work with f_PeasantCarriesResource

    m_movingToRessource->addTransition(new ConditionBoolean(f_isdead), pPeasant->m_pDead);
    m_returningRessource->addTransition(new ConditionBoolean(f_isdead), pPeasant->m_pDead);


    // When moving to the GoldMine or Townhall the Peasant can be interrupted with a new moving order
    m_movingToRessource->addTransition(EventID(GeneralActions::Move), pPeasant->m_pMoving);
    m_collectRessource->addTransition(EventID(GeneralActions::Move), pPeasant->m_pMoving);
    m_returningRessource->addTransition(EventID(GeneralActions::Move), pPeasant->m_pMoving);


    // Finally let's build the state circle: moving to resource -> collect resource -> move back resource -> deliver resource -> repeat ...
    m_movingToRessource->setNextState(m_collectRessource);
    m_collectRessource->setNextState(m_returningRessource);
    m_returningRessource->setNextState(m_deliverRessource);
    m_deliverRessource->setNextState(m_movingToRessource);

    m_pCurrState = m_movingToRessource;
}


CompositeStateChopWood::~CompositeStateChopWood() {
    delete m_deliverRessource;
    delete m_returningRessource;
    delete m_collectRessource;
    delete m_movingToRessource;
}


void CompositeStateChopWood::onEnter(const Event* pEvent) {
    assert(pEvent);

    // Store the initial wood position
    m_lastWoodPosition.x = pEvent->tileX();
    m_lastWoodPosition.y = pEvent->tileY();

    m_pCurrState->onEnter(pEvent);
}


State* CompositeStateChopWood::process() {
    State* oldState = m_pCurrState;
    m_pCurrState = m_pCurrState->process();

    // Debug state transitions
    if (oldState != m_pCurrState) {
        if (m_pCurrState == m_movingToRessource) {
            std::cerr << "DEBUG: Transitioning to MovingToResource state" << std::endl;
            // Call onEnter without event to trigger wood position logic
            m_pCurrState->onEnter(nullptr);
        } else if (m_pCurrState == m_collectRessource) {
            std::cerr << "DEBUG: Transitioning to CollectResource state" << std::endl;
        } else if (m_pCurrState == m_returningRessource) {
            std::cerr << "DEBUG: Transitioning to ReturningResource state" << std::endl;
        } else if (m_pCurrState == m_deliverRessource) {
            std::cerr << "DEBUG: Transitioning to DeliverResource state" << std::endl;
        }
    }

    return m_pCurrState;
}


void CompositeStateChopWood::setLastWoodPosition(const Point& pos) {
    m_lastWoodPosition = pos;
}


Point CompositeStateChopWood::getLastWoodPosition() const {
    return m_lastWoodPosition;
}
