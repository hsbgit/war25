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

#include <chrono>

namespace War25::Constants {
    // Game Logic Constants
    constexpr int TICKS_PER_SECOND = 40;
    constexpr int TICK_DURATION_MS = 1000 / TICKS_PER_SECOND;
    constexpr int MAX_FRAMESKIP = 10;

    // Default Window Settings
    constexpr int DEFAULT_WINDOW_WIDTH = 1600;
    constexpr int DEFAULT_WINDOW_HEIGHT = 1280;

    // Minimum Window Settings
    constexpr int MIN_WINDOW_WIDTH = 800;
    constexpr int MIN_WINDOW_HEIGHT = 600;

    // UI Constants
    constexpr int ACTIONS_PER_ROW = 3;
    constexpr int DEFAULT_BUILDING_ARMOR = 20;
    constexpr int DEFAULT_BUILDING_SIGHT = 3;
    constexpr int TOWER_SIGHT = 9;
    constexpr int TOWNHALL_SIGHT = 4;

    // Sound Debounce Settings
    constexpr auto SOUND_DEBOUNCE_INTERVAL = std::chrono::milliseconds(250);

    // File System
    inline const char* const PATH_SEPARATOR = "/";

    // ImGui Colors
    constexpr unsigned int ICON_BORDER_SELECTED = 0xFF962D1E;   // RGB(45,150,255) in ABGR
    constexpr unsigned int ICON_BORDER_UNSELECTED = 0x6696FA42; // RGB(66,150,250) with alpha 102

    // Game Balance
    constexpr int MAX_HEALTH = 999999;
    constexpr int CONSTRUCTION_PHASES = 3;
}