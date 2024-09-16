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

#include "EventBroker.h"
#include "EventProcessor.h"
#include <cassert>
#include <iostream>

EventBroker g_pEventBroker;


void EventBroker::registerEventHandler(EventProcessor* pEP) {
	m_mapEventProcessorIDsToObjectAddresses[pEP->getID()] = pEP;
	m_mapEventBuffer[pEP->getID()]; // Add key with empty vector to map

#ifdef _DEBUG
	std::cerr << "Registered EventProcessors: [" << m_mapEventProcessorIDsToObjectAddresses.size() << "]" << std::endl;
#endif
}


void EventBroker::deregisterEventHandler(EventProcessor* pEP) {
	m_mapEventBuffer.erase(pEP->getID());

	auto it = m_mapEventProcessorIDsToObjectAddresses.find(pEP->getID());
	assert(it != m_mapEventProcessorIDsToObjectAddresses.end());
	m_mapEventProcessorIDsToObjectAddresses.erase(it);

#ifdef _DEBUG
	std::cerr << "Registered EventProcessors: [" << m_mapEventProcessorIDsToObjectAddresses.size() << "]" << std::endl;
#endif
}


void EventBroker::addEvent(const Event& event) {
	assert(m_mapEventBuffer.contains(event.eventProcessorID()));
	m_mapEventBuffer[event.eventProcessorID()].push_back(event);
}


void EventBroker::dispatchEvents() {
	for (auto& [eventProcessorID, vecEvents] : m_mapEventBuffer) {
		assert(m_mapEventProcessorIDsToObjectAddresses.contains(eventProcessorID));

		for (auto& event : vecEvents) {
			m_mapEventProcessorIDsToObjectAddresses[eventProcessorID]->handleEvent(event);
		}

		vecEvents.clear();
	}

	m_setLockedEventProcessors.clear();
}


void EventBroker::lockEventProcessor(EventProcessorID id) {
	m_setLockedEventProcessors.insert(id);
}


bool EventBroker::isEventProcessorLocked(EventProcessorID id) const {
	return m_setLockedEventProcessors.contains(id);
}
