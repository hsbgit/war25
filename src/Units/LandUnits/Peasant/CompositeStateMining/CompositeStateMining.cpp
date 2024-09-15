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
#include "CompositeStateMining.h"
#include "EventHandling/StateIdle.h"
#include "StateMoveToGoldmine.h"
#include "StateCollectGold.h"
#include "StateReturningResource.h"
#include "StateEnterBuildingForDelivery.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include <cassert>


CompositeStateMining::CompositeStateMining(Peasant* pPeasant) : State(pPeasant) {
    m_movingToRessource = new StateMoveToGoldmine(pPeasant, pPeasant->m_pIdle);
    m_collectRessource = new StateCollectGold(pPeasant, pPeasant->m_pIdle);
    m_returningRessource = new StateReturningResource(pPeasant, pPeasant->m_pIdle);
    m_deliverRessource = new StateEnterBuildingForDelivery(pPeasant);


    // Add all transitions
    f_PeasantCarriesGold = [this]() {
        IResourceCollector* pPeasant = dynamic_cast<IResourceCollector*>(m_pOwner);
        return (pPeasant->currentlyCarrying() == ResourceType::Gold);
    };



    f_isdead = [this]() {
        return m_pOwner->isDead();
    };


    m_movingToRessource->addTransition(new ConditionBoolean(f_isdead), pPeasant->m_pDead);
    m_returningRessource->addTransition(new ConditionBoolean(f_isdead), pPeasant->m_pDead);


    // When moving to the GoldMine or Townhall the Peasant can be interrupted with a new moving order
    m_movingToRessource->addTransition(EventID(GeneralActions::Move), pPeasant->m_pMoving);
    m_returningRessource->addTransition(EventID(GeneralActions::Move), pPeasant->m_pMoving);


    // Peasant is in mine and finally gets is gold -> bring it home
    m_collectRessource->addTransition(new ConditionBoolean(f_PeasantCarriesGold), m_returningRessource);

    // Peasant was sent to mine but he already carries gold/wood -> bring it home
    m_movingToRessource->addTransition(new ConditionBoolean(f_PeasantCarriesGold), m_returningRessource);

    // wood -> state returnwood
    //m_movingToRessource->addTransition(new ConditionBoolean(f_PeasantCarriesWood), m_returningWood);



    // Finally let's build the state circle: moving to resource -> collect resource -> move back resource -> deliver resource -> repeat ...
    m_movingToRessource->setNextState(m_collectRessource);
    m_collectRessource->setNextState(m_returningRessource);
    m_returningRessource->setNextState(m_deliverRessource);
    m_deliverRessource->setNextState(m_movingToRessource);

    m_pCurrState = m_movingToRessource;
}


CompositeStateMining::~CompositeStateMining() {
    delete m_deliverRessource;
    delete m_returningRessource;
    delete m_collectRessource;
    delete m_movingToRessource;
}


void CompositeStateMining::onEnter(const Event* pEvent) {
    assert(pEvent);
    m_movingToRessource->onEnter(pEvent);
}


State* CompositeStateMining::process() {
    m_pCurrState = m_pCurrState->process();
    return m_pCurrState; // This?
}
