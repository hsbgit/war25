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
#include "EventHandlerID.h"

class IEventHandler;


class Event {
public:
	Event() { }


	// @param e: For EventID have a look at UnitActions.h. These values are casted into type EventID
	template<class T>
	Event(EventProcessorID receivingEventProcessor, T e, std::uint16_t optionalData = 0) : m_receivingEventProcessor(receivingEventProcessor), m_eventID(static_cast<EventID>(e)), m_optionalData(optionalData) {

	}

	// @param e: For EventID have a look at UnitActions.h. These values are casted into type EventID
	// @param tileX, tileY :Is an event related to a specific tile? E.g., when casting a whirlwind the user specifies the origin tile
	template<class T>
	Event(EventProcessorID receivingEventProcessor, T e, std::int32_t tileX, std::int32_t tileY, std::uint16_t optionalData = 0) : Event(receivingEventProcessor, e, optionalData) {
		m_tileX = tileX;
		m_tileY = tileY;
	}


	inline EventProcessorID eventProcessorID() const {
		return m_receivingEventProcessor;
	}

	inline EventID getEventID() const {
		return m_eventID;
	}


	inline std::int32_t tileX() const {
		return m_tileX;
	}

	inline std::int32_t tileY() const {
		return m_tileY;
	}


	inline bool operator ==(const Event& e) const {
		return m_eventID == e.m_eventID;
	}


	template<class T>
	inline bool operator ==(T eventEnumID) const {
		return m_eventID == static_cast<std::uint16_t>(eventEnumID);
	}

private:
	EventProcessorID m_receivingEventProcessor = 0;
	EventID m_eventID = 0;
	std::uint16_t m_optionalData = 0;
	std::int32_t m_tileX = -1, m_tileY = -1;
};
