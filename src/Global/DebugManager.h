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

class DebugManager {
public:
    bool m_map_drawTileInformation = false; // Water, Land, Destroyable, Occupied
    bool m_map_considerExploration = false;
    bool m_map_drawFogOfWar = false;

    bool m_astar_showOpenList = false;
    bool m_astar_showClosedList = false;
    bool m_astar_showPath = false;    

    bool m_object_drawVisibilityRanges = false; // sight (blue) and attack range (red)
    bool m_object_allowNonAlliedSelection = true;

    bool m_ignoreFood = true;

    bool m_object_vanishing = true;
};

extern DebugManager gDebugManager; // ToDo: Secure with #ifdef DEBUG
