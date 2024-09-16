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
#include "Action.h"
#include "TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"
#include <cassert>
#include "EventHandling/EventBroker.h"


Action::Action() : m_ActionType(ActionType::EmptyAction), m_icon(g_pRessourceManager->getUISprite("orc//cursors//red_crosshairs.png")) {

}

Action::Action(EventProcessorID eventProcessor, EventID e, const Sprite& icon, ActionType actiontype) : m_ActionType(actiontype), m_eventProcessor(eventProcessor), m_eventID(e), m_icon(icon) {

}


void Action::trigger() {
	g_pEventBroker.addEvent(Event(m_eventProcessor, m_eventID));
}



void Action::trigger(const Point& tile_world) {
	g_pEventBroker.addEvent(Event(m_eventProcessor, m_eventID, tile_world.x, tile_world.y));
}

