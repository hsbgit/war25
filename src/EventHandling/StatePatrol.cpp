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

#include "StatePatrol.h"
#include "EventHandling/StateMoving.h"
#include "Units/Unit.h"


CompositeStatePatrol::CompositeStatePatrol(Unit* pUnit) : State(pUnit) {
    m_movingToStartPoint = new StateMoving(pUnit);
    m_movingToPatrolPoint = new StateMoving(pUnit);

    m_movingToStartPoint->setNextState(m_movingToPatrolPoint);
    m_movingToPatrolPoint->setNextState(m_movingToStartPoint);

    m_pCurrState = m_movingToPatrolPoint;
}


void CompositeStatePatrol::onEnter(const Event* pEvent) {
    assert(pEvent);

    Point startPoint = m_pOwner->getTile()->getPos();
    m_movingToStartPoint->setTargetTile(startPoint);

    // Patrol point was set by the user and is part of pEvent
    m_pCurrState->onEnter(pEvent);
}


State* CompositeStatePatrol::process() {
    // State* pPotentialNextState = State::checkForTransition();

    m_pCurrState = m_pCurrState->process();
    return this;
}
