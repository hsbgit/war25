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
#pragma once

#include "EventHandling/Event.h"
#include "StateCondition.h"

#include <map>
#include <functional>

class Object;


class State {
	using Callback = std::function<void()>;
	Callback m_callback;

public:
	State(Object* pOwner);

	virtual ~State() {
		for (auto& pair : m_transitionConditions) {
			delete pair.first;  // Delete condition objects
	   //     delete pair.second;  // States are managed externally, do not delete them here
		}
	}

	inline void registerCallback(const Callback& callback) {
		m_callback = callback;
	}

	virtual void onEnter(const Event* pEvent = nullptr) { }

	virtual void onExit() {
		if (m_callback) {
			m_callback();
		}
	};


	// Can be set if a state may finish internally. 
	// In this case, the state can call 'transitionToNextState' in 'process' 
	void setNextState(State* pNextState) {
		m_pNextState = pNextState;
	}


	virtual State* process() = 0;

	State* checkForTransition(const Event& event);
	State* checkForTransition();


	inline void addTransition(StateCondition* pCond, State* pNextState) {
		m_transitionConditions[pCond] = pNextState;
	}

	inline void addTransition(EventID id, State* pNextState) {
		m_transitionEvents[id] = pNextState;
	}

protected:
	// Individual events and transition may lead to a transition to a new state
	std::map<EventID, State*> m_transitionEvents;
	std::map<StateCondition*, State*> m_transitionConditions;

	State* transitionToNextState();

	Object* m_pOwner = nullptr;

private:
	State* m_pNextState = nullptr;
};
