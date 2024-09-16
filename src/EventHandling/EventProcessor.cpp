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
#include "EventProcessor.h"
#include "EventBroker.h"


EventProcessorID EventProcessor::m_idCounter = 1; // We start at 1 such that 0 indicates a non-set EventProcessor in class 'Event'


EventProcessor::EventProcessor() {
    m_id = EventProcessor::m_idCounter++;
    g_pEventBroker.registerEventHandler(this);
}


EventProcessor::~EventProcessor() {
    g_pEventBroker.deregisterEventHandler(this);
}

