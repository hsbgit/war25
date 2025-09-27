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
#include <set>
#include <vector>
#include <SDL.h>
#include "GUI/Window.h"
#include "GUI/MusicPlayer.h"
#include "GUI/PUDBrowser.h"
#include "GUI/imgui/imgui.h"
#include "GUI/DebugWindow.h"
#include "GUI/PlayerStatsWindow.h"
#include "GUI/MenuBarWindow.h"
#include "GUI/ObjectInfoWindow.h"
#include "GUI/DisplaySettingsWindow.h"


namespace gui {
    class MiniMap;
}


class Game {
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration time = end_time - start_time;


     float avg = 0.0;
    unsigned long cnt = 0;

public:
    explicit Game(SDL_Window* window, const std::string& strPathToPUDFile);
    ~Game() = default;

    // Prevent copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool quitRequested() const noexcept {
        return m_quitRequested;
    }

    const std::string newMapRequested() const {
        return wPUDBrowser.selectedMap();
    }

    void doEventHandling();
    void doUpdate();
    void doRendering();
    void applyDisplaySettings();


private:
    void debugRenderAStar();

    void handleUnitGrouping(const Uint8* keyboardState);

    struct InputState {
        bool rectDraggingActive = false;
        SDL_Point leftClickPosition = {0, 0};
    } m_inputState;

    bool m_quitRequested = false;


    SDL_Window* m_window = nullptr;
    int m_windowWidth = 0, m_windowHeight = 0;

    // Windows ------------
    gui::DebugWindow wDebug;
    gui::PlayerStatsWindow wPlayer;
    gui::MenuBarWindow wMenuBar;
    gui::PUDBrowser wPUDBrowser;
    gui::MiniMap* m_pMiniMap = nullptr;

    gui::MusicPlayer wMusicPlayer;
    gui::DisplaySettingsWindow wDisplaySettings;

    gui::ObjectInfoWindow m_infoWindow;
    std::set<Object*> m_selectedObjects;
    std::vector<std::set<Object*>> m_savedUserSelections; // m_selectedObjects can be saved with ctrl+1-9 and restored with 1-9


    struct DebugSettings {
        bool showDemoWindow = true;
        bool showAnotherWindow = false;
        ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    } m_debugSettings;

    static constexpr size_t MAX_SAVED_SELECTIONS = 10;
};
