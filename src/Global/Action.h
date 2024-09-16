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

#include "Sprite.h"
#include "Types.h"
#include "EventHandling/EventHandlerID.h"

class Object;
class ResourceManager_wartools;
class TechnologyManager;


class Action {
public:
    // SubMenu1 and SubMenu2 are only used by class Peasant (=the build menus)
    enum class ActionType {EmptyAction, SubMenu1, SubMenu2, TriggerOnly, WithWorldTile};  
    
    Action();
    Action(EventProcessorID eventProcessor, EventID e, const Sprite& icon, ActionType actiontype = ActionType::TriggerOnly);

    void trigger();
    void trigger(const Point& tile_world);

    inline ActionType getActionType() const {
        return m_ActionType;
    }

    inline EventProcessorID getEventProcessorID() const {
        return m_eventProcessor;
    }

    inline EventID getEventID() const {
        return m_eventID;
    }

    inline const Sprite& icon() const {
        return m_icon;
    }

private:
    ActionType m_ActionType;

    EventProcessorID m_eventProcessor;
    EventID m_eventID;

    const Sprite m_icon;
};
