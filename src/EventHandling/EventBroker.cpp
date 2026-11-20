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
#include <algorithm>
#include <cassert>
#include <iostream>

EventBroker g_pEventBroker;


void EventBroker::registerEventHandler(EventProcessor* pEP) {
	m_mapEventProcessorIDsToObjectAddresses[pEP->getID()] = pEP;

#ifdef _DEBUG
	std::cerr << "Registered EventProcessors: [" << m_mapEventProcessorIDsToObjectAddresses.size() << "]" << std::endl;
#endif
}


void EventBroker::deregisterEventHandler(EventProcessor* pEP) {
	auto it = m_mapEventProcessorIDsToObjectAddresses.find(pEP->getID());
	assert(it != m_mapEventProcessorIDsToObjectAddresses.end());
	m_mapEventProcessorIDsToObjectAddresses.erase(it);

#ifdef _DEBUG
	std::cerr << "Registered EventProcessors: [" << m_mapEventProcessorIDsToObjectAddresses.size() << "]" << std::endl;
#endif
}


void EventBroker::addEvent(const Event& event) {
	addEvent(m_localParticipant, commandTick(), event);
}

EventBroker::Tick EventBroker::commandTick() const noexcept {
	Tick tick = nextTick();
	while (true) {
		auto frame = m_frames.find(tick);
		if (frame == m_frames.end() ||
			!frame->second.completedParticipants.contains(m_localParticipant)) {
			return tick;
		}
		++tick;
	}
}

void EventBroker::addEvent(ParticipantID participant, Tick tick, const Event& event) {
	assert(m_participants.contains(participant));
	assert(tick > m_currentTick);
	assert(m_mapEventProcessorIDsToObjectAddresses.contains(event.eventProcessorID()));
	CommandFrame& frame = m_frames[tick];
	assert(!frame.completedParticipants.contains(participant));
	frame.commands[participant].push_back(event);
}

void EventBroker::dispatchEvents() {
	completeFrame(m_localParticipant, nextTick());
	(void)advanceTick();
}

void EventBroker::completeFrame(ParticipantID participant, Tick tick) {
	assert(m_participants.contains(participant));
	assert(tick > m_currentTick);
	m_frames[tick].completedParticipants.insert(participant);
}

void EventBroker::completeLocalFrame() {
	completeFrame(m_localParticipant, nextTick());
}

bool EventBroker::canAdvance() const {
	auto frame = m_frames.find(nextTick());
	if (frame == m_frames.end()) return false;
	return std::includes(frame->second.completedParticipants.begin(),
		frame->second.completedParticipants.end(), m_participants.begin(), m_participants.end());
}

bool EventBroker::advanceTick() {
	if (!canAdvance()) return false;
	dispatchFrame(nextTick());
	++m_currentTick;
	return true;
}

void EventBroker::dispatchFrame(Tick tick) {
	auto frameIt = m_frames.find(tick);
	assert(frameIt != m_frames.end());

	// std::map provides a stable participant order; vectors preserve command order.
	for (auto& [participant, events] : frameIt->second.commands) {
		(void)participant;
		for (const Event& event : events) {
			auto processor = m_mapEventProcessorIDsToObjectAddresses.find(event.eventProcessorID());
			if (processor != m_mapEventProcessorIDsToObjectAddresses.end()) {
				processor->second->handleEvent(event);
			}
		}
	}
	m_frames.erase(frameIt);
	m_lockedEventProcessors.erase(tick);
}

void EventBroker::configureParticipants(const std::set<ParticipantID>& participants,
	ParticipantID localParticipant) {
	assert(!participants.empty());
	assert(participants.contains(localParticipant));
	assert(m_frames.empty());
	m_participants = participants;
	m_localParticipant = localParticipant;
}

void EventBroker::reset() {
	m_frames.clear();
	m_lockedEventProcessors.clear();
	m_participants = {0};
	m_localParticipant = 0;
	m_currentTick = 0;
}


void EventBroker::lockEventProcessor(EventProcessorID id) {
	m_lockedEventProcessors[commandTick()].insert(id);
}


bool EventBroker::isEventProcessorLocked(EventProcessorID id) const {
	auto frame = m_lockedEventProcessors.find(commandTick());
	return frame != m_lockedEventProcessors.end() && frame->second.contains(id);
}
