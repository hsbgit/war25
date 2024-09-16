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
#include "StateMachine.h"
#include "State.h"
#include "StateMoving.h"
#include <cassert>


StateMachine::StateMachine(State* pCurrState) : m_pCurrState(pCurrState) {

}


void StateMachine::process() {
	assert(m_pCurrState);
	m_pCurrState = m_pCurrState->checkForTransition();
	m_pCurrState = m_pCurrState->process(); // State may finish and switch to its next state. See 'State::transitionToNextState'
}


void StateMachine::handleEvent(const Event& event) {
	assert(m_pCurrState);
	m_pCurrState = m_pCurrState->checkForTransition(event);
}


bool StateMachine::isCurrentStateMovingState() const {
	return dynamic_cast<StateMoving*>(m_pCurrState) ? true : false;
}


State* State::checkForTransition(const Event& event) {
	// No priority, if there are several possible transitions simply pick the first one
	if (m_transitionEvents.contains(event.getEventID())) {
		State* pNextState = m_transitionEvents.at(event.getEventID());
		this->onExit();
		pNextState->onEnter(&event);
		return pNextState;
	}

	return this;
}


State* State::checkForTransition() {
	// No priority, if there are several possible transitions simply pick the first one
	for (auto& [condition, state] : m_transitionConditions) {
		if (condition->isTrue()) {
			this->onExit();
			state->onEnter();
			return state;
		}
	}

	return this;
}
