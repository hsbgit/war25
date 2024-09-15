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
const int windowWidth = 1600;
const int windowHeight = 1280;

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include <map>
#include <vector>
#include <cassert>
#include <string>
#include <cstdint>
#include <typeindex>
#include <typeinfo>
#include <iostream>

#include "Game.h"

// ToDo: Common header which includes all buildings, units etc. at once?
#include "Buildings/Farm.h"
#include "Buildings/Barracks.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Townhall.h"
#include "Buildings/LumberMill.h"
#include "Buildings/ScoutTower.h"


#include "Player.h"
#include "Global/Object.h"
#include "Global/DebugManager.h"
#include "Map/AStar.h"
#include "Renderer.h"
#include "Resources/SoundManager.h"
#include "Global/GlobalAttributes.h"

// ----------------------------------------------------------------------------------------------------
bool buildMode = false;
bool quit = false;

#include "Resources/resourcemanager_wartools.h"


int buildingType = 0; // rathaus, barracks, scout tower, farm, lumbermill, blacksmith

Player* pHumanPlayer = nullptr;
std::size_t iSelected = 0;
// ----------------------------------------------------------------------------------------------------



#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif




#include <chrono>
#include <war2.h>

#include "GUI/imgui/imgui_impl_sdl2.h"
#include "GUI/imgui/imgui_impl_sdlrenderer2.h"


// SDL2 is doing weird things by having (under windows) somewhere a "#define main"
// We have to undefine it, otherwise we get a linker error as main cannot be found under windows
#ifdef main
#undef main
#endif 

// Main code
int main(int, char**)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr<<"Error: "<< SDL_GetError() << std::endl;
        return -1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL2_image could not be initialized: " << IMG_GetError() << std::endl;
        assert(0);
        // ToDo: Exception
    }


 
    // https://dewitters.com/dewitters-gameloop/
    // The following variables must be initialized after SDL_Init since 'g_tick_ms' is global and
    // further code relies on it
    const int TICKS_PER_SECOND = 40; // Currently 25ms per tick as 1000ms / 40 ticks = 25ms/tick
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    static_assert(1000 % TICKS_PER_SECOND == 0);
    const int TICK_DURATION_MS = SKIP_TICKS;
    const int MAX_FRAMESKIP = 10;

    float gameSpeedFactor = 1.0;

    g_tick_ms = (int)(TICK_DURATION_MS * gameSpeedFactor);
    std::cout << "g_tick_ms: " << g_tick_ms << std::endl;


    SDL_Window *window = SDL_CreateWindow("War 2.5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           windowWidth, windowHeight, SDL_WINDOW_OPENGL);

    g_pRenderer = new Renderer(window);
    SDL_Renderer* renderer = g_pRenderer->accessSDLRenderer();

    // Pointers as they must be initialized after the SDL_Init call
    g_pSoundManager = new SoundManager();
    g_pRessourceManager = new ResourceManager_wartools("data//graphics", g_pRenderer);
    war2_init();

  
    // ToDo: switching to winter required as wasteland and winter have IDs which are not part of summer 
     g_pRessourceManager->switchSeason(Season::Winter);
 


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();


    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    //ImGui_ImplSDLRenderer_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash / !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:/Windows/Fonts/ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);


    std::string strPathToPudFile = "data/maps/Islands in the stream modified.pud";
    //std::string strPathToPudFile = "data/maps/ClanEviLMapPackv1.0/ClanEviLMPv1.0/PBall/8 ALLeRiA    ScArLeT.pud"; // Viewport test
    //std::string strPathToPudFile = "data/maps/ClanEviLMapPackv1.0/ClanEviLMPv1.0/Other/8 ALeRiA WiDe ScArLeT.pud"; // Lag test


    
    while (strPathToPudFile.size()) {
        Game game(window, strPathToPudFile);

        int loops;
        auto next_game_tick = std::chrono::high_resolution_clock::now();

        // Actual main loop
        while (!game.quitRequested()) {
            loops = 0;

            auto currTime = std::chrono::high_resolution_clock::now();
            while (currTime > next_game_tick && loops < MAX_FRAMESKIP) {
                game.doEventHandling();
                game.doUpdate();

                next_game_tick += std::chrono::milliseconds(g_tick_ms);
                loops++;
            }

            if (loops >= MAX_FRAMESKIP) {
                std::cerr << "SKIPPED TOO MANY FRAMES" << std::endl;
            }

            game.doRendering();
        }

        strPathToPudFile = game.newMapRequested();
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
