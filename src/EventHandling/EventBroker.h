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

#include <cstdint>
#include <map>
#include <set>
#include <vector>

#include "Event.h"
#include "EventHandlerID.h"

class EventProcessor;

class EventBroker
{
public:
	using Tick = std::uint64_t;
	using ParticipantID = std::uint16_t;

	void registerEventHandler(EventProcessor* pEP);
	void deregisterEventHandler(EventProcessor* pEP);

	virtual void addEvent(const Event& event);
	void addEvent(ParticipantID participant, Tick tick, const Event& event);

	/*
	 * Distribute all events for the current lockstep
	 * to all EventProcessors
	 */
	virtual void dispatchEvents();

	// Marks a participant's command frame as complete. advanceTick() only
	// executes a frame after every participant has completed it.
	void completeFrame(ParticipantID participant, Tick tick);
	void completeLocalFrame();
	bool advanceTick();
	bool canAdvance() const;
	Tick currentTick() const noexcept { return m_currentTick; }
	Tick nextTick() const noexcept { return m_currentTick + 1; }
	Tick commandTick() const noexcept;

	void configureParticipants(const std::set<ParticipantID>& participants,
		ParticipantID localParticipant = 0);
	void reset();


	/*
	 * When clicking an action the event could immediately be processed
	 * by the EventProcessor. Due to lockstep we have to collect all events and dispatch them later
	 * in a bunch.
	 *
	 * To avoid generating more than one event per EventProcessor per lockstep this methods are used.
	 *
	 * Locks are scoped to the command frame and are cleared when that tick is dispatched.
	 */
	void lockEventProcessor(EventProcessorID id);
	bool isEventProcessorLocked(EventProcessorID id) const;


	inline EventProcessor* getEventHandler(EventProcessorID id) {
		auto it = m_mapEventProcessorIDsToObjectAddresses.find(id);
		return it == m_mapEventProcessorIDsToObjectAddresses.end() ? nullptr : it->second;
	}

private:
	struct CommandFrame {
		std::map<ParticipantID, std::vector<Event>> commands;
		std::set<ParticipantID> completedParticipants;
	};

	void dispatchFrame(Tick tick);

	std::map<Tick, std::set<EventProcessorID>> m_lockedEventProcessors;
	std::map<EventProcessorID, EventProcessor*> m_mapEventProcessorIDsToObjectAddresses;
	std::map<Tick, CommandFrame> m_frames;
	std::set<ParticipantID> m_participants{0};
	ParticipantID m_localParticipant = 0;
	Tick m_currentTick = 0;
};

extern EventBroker g_pEventBroker;
