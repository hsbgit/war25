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

#include <set>
#include <vector>
#include <unordered_map>

#include "Event.h"
#include "EventHandlerID.h"

class EventProcessor;

class EventBroker
{
public:
	void registerEventHandler(EventProcessor* pEP);
	void deregisterEventHandler(EventProcessor* pEP);

	virtual void addEvent(const Event& event);

	/*
	 * Distribute all events for the current lockstep
	 * to all EventProcessors
	 */
	virtual void dispatchEvents();


	/*
	 * When clicking an action the event could immediately be processed
	 * by the EventProcessor. Due to lockstep we have to collect all events and dispatch them later
	 * in a bunch.
	 *
	 * To avoid generating more than one event per EventProcessor per lockstep this methods are used.
	 *
	 * A call to dispatchEvents() clears the associated set.
	 */
	void lockEventProcessor(EventProcessorID id);
	bool isEventProcessorLocked(EventProcessorID id) const;


	inline EventProcessor* getEventHandler(EventProcessorID id) {
		if (m_mapEventProcessorIDsToObjectAddresses.contains(id))
			return m_mapEventProcessorIDsToObjectAddresses[id];

		return nullptr;
	}

private:
	std::set<EventProcessorID> m_setLockedEventProcessors;

	std::unordered_map<EventProcessorID, EventProcessor*> m_mapEventProcessorIDsToObjectAddresses;
	std::unordered_map<EventProcessorID, std::vector<Event>> m_mapEventBuffer;
};

extern EventBroker g_pEventBroker;
