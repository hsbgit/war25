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

#include <queue>
#include <vector>
#include <cassert>
#include <cstdint>

#include "Event.h"
#include "EventHandlerID.h"

class Object;
class Player;


class EventProcessor
{
public:
	EventProcessor();
	virtual ~EventProcessor();

	inline EventProcessorID getID() const {
		return m_id;
	}

	/*
	 * EventHandlers can decide by themselves whether to queue an event
	 * (by using 'addToEventQueue') or to process it immediately
	 * when "handleEvent" is called
	 *
	 * erst handleevents dann process(dt) aufrufen
	 */
	virtual void handleEvent(const Event& event) = 0;


	virtual bool isEventInProgress() const {
		return m_eventHandlingInProgress;
	}

protected:
	inline void setEventInProgress(bool inProgress) {
		m_eventHandlingInProgress = inProgress;
	}

private:
	bool m_eventHandlingInProgress = false;

	EventProcessorID m_id = 0;
	static EventProcessorID m_idCounter; // Additional ID is required to connect events to objects since object heap addresses will most likely differ within network games
};
