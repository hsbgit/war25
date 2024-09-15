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


CompositeStateChopWood::CompositeStateChopWood(Peasant* pPeasant) : State(pPeasant) {
    m_movingToRessource = new StateMoveToWood(pPeasant, pPeasant->m_pIdle);
    m_collectRessource = new StateLumberWood(pPeasant, pPeasant->m_pIdle);
    m_returningRessource = new StateReturningResource(pPeasant, pPeasant->m_pIdle);
    m_deliverRessource = new StateEnterBuildingForDelivery(pPeasant);


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
    m_pCurrState->onEnter(pEvent);
}


State* CompositeStateChopWood::process() {
    m_pCurrState = m_pCurrState->process();
    return m_pCurrState;
}
