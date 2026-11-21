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
#include "StateIdle.h"
#include "Global/GlobalAttributes.h"
#include "Global/PseudoRandom.h"
#include "Units/Unit.h"

StateIdle::StateIdle(Object* pOwner) : State(pOwner) {
	scheduleNextTurn();
}

void StateIdle::onEnter(const Event* pEvent) {
	if (auto* pUnit = dynamic_cast<Unit*>(m_pOwner))
		pUnit->setIdleAnimation(m_facing);
	scheduleNextTurn();
}


void StateIdle::onExit() {
}


State* StateIdle::process()  {
	// Vessels and aircraft keep their commanded heading while idle. Their
	// sprite animation provides movement (bob, rotor, wings) without random
	// course changes.
	if (!m_pOwner->isLandObject()) return this;
	if (m_ticksUntilTurn > 0) {
		--m_ticksUntilTurn;
		return this;
	}

	const int step = PseudoRandom::cosmeticRnd(0, 1) == 0 ? -1 : 1;
	const int direction = (static_cast<int>(m_facing) + step + 8) % 8;
	m_facing = static_cast<Direction>(direction);
	if (auto* pUnit = dynamic_cast<Unit*>(m_pOwner))
		pUnit->setIdleAnimation(m_facing);

	scheduleNextTurn();
    return this;
}

void StateIdle::scheduleNextTurn() {
	const int minTicks = calculateRequiredTicksForGameAction(2500);
	const int maxTicks = calculateRequiredTicksForGameAction(7500);
	m_ticksUntilTurn = PseudoRandom::cosmeticRnd(minTicks, maxTicks);
}
