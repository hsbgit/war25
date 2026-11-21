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
#include "Units/IArmed.h"


CompositeStatePatrol::CompositeStatePatrol(Unit* pUnit) : State(pUnit) {
    m_movingToStartPoint = new StateMoving(pUnit);
    m_movingToPatrolPoint = new StateMoving(pUnit);

    m_movingToStartPoint->setNextState(m_movingToPatrolPoint);
    m_movingToPatrolPoint->setNextState(m_movingToStartPoint);

    m_pCurrState = m_movingToPatrolPoint;
}

CompositeStatePatrol::~CompositeStatePatrol() {
	delete m_movingToStartPoint;
	delete m_movingToPatrolPoint;
}


void CompositeStatePatrol::onEnter(const Event* pEvent) {
    assert(pEvent);

    Point startPoint = m_pOwner->getTile()->getPos();
    m_movingToStartPoint->setTargetTile(startPoint);

	// A fresh patrol command always starts by walking to the clicked point.
	// Without this reset, re-entering patrol could head to the stale start point.
	m_pCurrState = m_movingToPatrolPoint;
	m_pausedForCombat = false;
    m_pCurrState->onEnter(pEvent);
}


State* CompositeStatePatrol::process() {
	auto* armed = dynamic_cast<IArmed*>(m_pOwner);
	if (armed && armed->isArmed()) {
		Object* target = armed->activeTarget();
		if (target && (target->isDead() || !armed->activeTargetInRange(m_pOwner))) {
			armed->clearActiveTarget();
			target = nullptr;
		}
		if (!target) {
			const auto targets = armed->findTargetObject(m_pOwner);
			if (!targets.empty()) {
				target = *targets.begin();
				armed->assignTarget(target);
			}
		}
		m_pausedForCombat = target && armed->activeTargetInRange(m_pOwner);
		if (m_pausedForCombat) return this;
	}
	m_pausedForCombat = false;

    m_pCurrState = m_pCurrState->process();
    return this;
}
