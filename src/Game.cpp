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


// ------------------------------------------------------------------------
// !!! ToDo: This file needs a big refactoring !!!
// ------------------------------------------------------------------------


#include "Game.h"
#include "Renderer.h"
#include "EventHandling/EventBroker.h"
#include "Resources/SoundManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Map/Map.h"
#include "Global/Viewport.h"
#include "Global/DebugManager.h"
#include "Map/AStar.h" // Just for debug rendering purposes
#include "Global/GlobalAttributes.h"
#include "Units/Critter.h"
#include "Buildings/Goldmine.h"
#include "GUI/MiniMap.h"
#include "Player.h" // Debug, can be removed
#include <iostream>


// For non virtual box maybe better?
//#include "GUI/imgui/imgui_impl_opengl3_loader.h"
//#include "GUI/imgui/imgui_impl_opengl3.h"

#include "GUI/imgui/imgui_impl_sdl2.h"
#include "GUI/imgui/imgui_impl_sdlrenderer2.h"

#include <stdio.h>
#include "GUI/imgui/gl3w/GL/gl3w.h"   // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL_opengl.h>


// ToDo: Where to move this?
Map* pMap = nullptr;


#include "GUI/DebugSpellWindow.h"
#include "GUI/DebugWindow.h"
#include "GUI/MenuBarWindow.h"
#include "GUI/MiniMap.h"
#include "GUI/ObjectInfoWindow.h"
#include "GUI/PlayerStatsWindow.h"
#include "Magic/SpellExecutor.h"


#undef min // Dear windows.h (somewhere automatically included)
#undef max // This sucks
#include <algorithm>


gui::DebugSpellWindow gSpellWindow;



Game::Game(SDL_Window* window, const std::string& strPathToPUDFile) : window(window), m_infoWindow(m_selectedObjects) {
    SDL_GetWindowSize(window, &m_windowWidth, &m_windowHeight);
    g_pRessourceManager->setCursor(CursorTypes::HumanGauntlet);

    // Must be created at first 
    pMap = new Map(strPathToPUDFile, g_pRessourceManager); 

    gViewport = Viewport(pMap->sideLength(), m_windowWidth, m_windowHeight);
    gViewport.setViewPortPosition(pMap->getHumanPlayerStartPosition());
    pMap->renderMiniMapOverlay(&gViewport);
    m_pMiniMap = new gui::MiniMap(pMap, &gViewport);


    wPlayer = gui::PlayerStatsWindow(pMap->humanControlledPlayer());
    wMenuBar = gui::MenuBarWindow(pMap->humanControlledPlayer());
    
    m_savedUserSelections.resize(10);

    // Debug
    gSpellExecutor.setMap(pMap);
    gSpellWindow.setMap(pMap);

    // Debug
    pMap->humanControlledPlayer()->addToResources(Resources(99999, 99999, 99999));
}


// ToDo: Sampling does not always work...
void Game::handleUnitGrouping(const Uint8* keyboardState) {
    // Update all saved selections -> if an object is not selectable anymore it may be dead
    for (auto& savedSelection : m_savedUserSelections) {
        std::erase_if(savedSelection, [](auto& pObject) {
            return !pObject->isSelectable();
            });
    }

    // Restoring a saved group
    for (int key = SDL_SCANCODE_1; key <= SDL_SCANCODE_9; ++key) {
        if (keyboardState[key] && !(keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL])) {
            m_selectedObjects = m_savedUserSelections[key - SDL_SCANCODE_1];
        }        
    }

    // Saving a new group (only if at least one unit is selected)
    if (!m_selectedObjects.size())
        return;

    for (int key = SDL_SCANCODE_1; key <= SDL_SCANCODE_9; ++key) {
        if (keyboardState[key] && (keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL])) {
            m_savedUserSelections[key - SDL_SCANCODE_1] = m_selectedObjects;
        }
    }
}


void Game::doEventHandling() {
    g_pEventBroker.dispatchEvents();

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            m_quitRequested = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            m_quitRequested = true;

  
        if (event.window.event == SDL_WINDOWEVENT_ENTER) {
            g_pRessourceManager->refreshLoadedCursor();
        }

        // ToDo: Triggered, but setting cursor does not work?
        if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
    //        g_pRessourceManager->setCursor(CursorTypes::HumanGauntlet);
        }


        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_s) {
                g_pRessourceManager->switchSeason(Season::Summer);
            }
            else if (event.key.keysym.sym == SDLK_d) {
                g_pRessourceManager->switchSeason(Season::Swamp);
            }
            else if (event.key.keysym.sym == SDLK_w) {
                g_pRessourceManager->switchSeason(Season::Winter);
            }
            else if (event.key.keysym.sym == SDLK_a) {
                g_pRessourceManager->switchSeason(Season::Wasteland);
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                m_selectedObjects.clear();
            }
        }

        if (event.type == SDL_MOUSEMOTION) {
            Point posMouse = { event.motion.x, event.motion.y };

            Point tile_world = gViewport.screenToWorld_tile(gViewport.pixelToTile(posMouse));
            Object* pObject = pMap->getObjectOnTile(tile_world);

            if (pObject)
                g_pRessourceManager->setCursor(CursorTypes::MagnifyingGlass);
            else
                g_pRessourceManager->setCursor(CursorTypes::HumanGauntlet);

        }


        if (gViewport.consume(event))
            continue;

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
         
        }
        else {
            // Only called once when clicked
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    Point posClicked = { event.button.x, event.button.y };
                    posLeftClicked = posClicked;

                    if (!m_infoWindow.isActionButtonActive())
                        rectDraggingActive = true;

                    // Debug
                    Point tile_world = gViewport.screenToWorld_tile(gViewport.pixelToTile(posClicked));
                    std::cout << std::dec << "Tile id: " << pMap->getTile(tile_world)->getTileID() << std::endl;
                    Object* pObject = pMap->getObjectOnTile(tile_world);


                    if (pObject && pObject->isSelectable()) {     

                    }
                }
            } 

            handleUnitGrouping(keyboardState);

         
            if (event.type == SDL_MOUSEBUTTONUP) {
                g_pSoundManager->playWAV("ui/click.wav");

                if (event.button.button == SDL_BUTTON_RIGHT) {
                    Point posClicked = { event.button.x, event.button.y };
                    Point tile_world = gViewport.screenToWorld_tile(gViewport.pixelToTile(posClicked));
       
                    m_infoWindow.onRightClicked(tile_world);
                    wDebug.setCurrentTile(pMap->getTile(tile_world));
                }


                if (event.button.button == SDL_BUTTON_LEFT) {
                    rectDraggingActive = false; // User made a selection

                    Point posClicked = { event.button.x, event.button.y };
                    Point tile_world = gViewport.screenToWorld_tile(gViewport.pixelToTile(posClicked));
                    gSpellWindow.onLeftClicked(tile_world);


                    if (m_infoWindow.isActionButtonActive()) 
                        m_infoWindow.onLeftClicked(tile_world);     
                    else {
                        Point posLeftReleased = { event.button.x, event.button.y };

                        Point points[2] = { posLeftClicked, posLeftReleased };
                        Rect selectionRect;
                        SDL_EnclosePoints(points, 2, nullptr, &selectionRect);

                        // Re-use array...
                        // Verify that click was released inside application window
                        int x = selectionRect.x;
                        int y = selectionRect.y;
                        points[0].x = (selectionRect.x < 0) ? 0 : selectionRect.x;
                        points[0].y = (selectionRect.y < 0) ? 0 : selectionRect.y;

                        int w = points[0].x + selectionRect.w;
                        int h = points[0].y + selectionRect.h;

                        // If x or y are cutted because they are located outsite our application window
                        // (negative coordinates) we have to substract them from w and h of the selection rectangle.
                        // Otherwise the selection rectangle gets too big as we shift x and to 0 but w and h stay the same
                        if (x < 0) {
                            w += x;
                        }

                        if (y < 0) {
                            h += y;
                        }

                        // ToDo: Bug       
                        // Map may be smaller than the Window/Viewport!
                        int mapWidth_px = gViewport.getMaxViewableMapWidth_tiles() * Tile::TileWidth_px - 1; // Start counting from 0. Otherwise, e.g., a map with width 32 would result in 32*32 = 1024 viewable pixels. Method pixelToTile would map this to tile 32 instead of 31 -> crash
                        int mapHeight_px = gViewport.getMaxViewableMapHeight_tiles() * Tile::TileWidth_px - 1;

                        points[1].x = std::min(mapWidth_px, w); 
                        points[1].y = std::min(mapHeight_px, h);


                        // Get tiles on screen and convert them to world tiles
                        Point posLeftClicked_tile = gViewport.screenToWorld_tile(gViewport.pixelToTile(points[0]));
                        Point posLeftReleased_tile = gViewport.screenToWorld_tile(gViewport.pixelToTile(points[1]));


                        // https://stackoverflow.com/questions/2620862/using-custom-stdset-comparator
                        std::set<Object*> userSelection = pMap->getObjects(posLeftClicked_tile, posLeftReleased_tile);

                        std::erase_if(userSelection, [](auto& pObject) {
                            return !pObject->isSelectable();
                            });

                        if (userSelection.size()) {
                            // Pressing CTRL keys adds unit the current selection, if any
                            if (m_selectedObjects.size() && (keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL])) {
                                if (userSelection.size() == 1 && m_selectedObjects.contains(*userSelection.begin())) {
                                    m_selectedObjects.erase(*userSelection.begin());
                                }
                                else {
                                    m_selectedObjects.insert(userSelection.begin(), userSelection.end());
                                    (*userSelection.begin())->onClicked(); // If units are added play at least one of their sounds
                                }
                            }
                            else {
                                m_selectedObjects = userSelection;
                                (*m_selectedObjects.begin())->onClicked(); // If this is an entirely new selection simply play the sound of the first unit
                            }
                        }
                    }
                }
            }
        }
    }
}


void Game::doUpdate() {
    auto start = std::chrono::high_resolution_clock::now();

    // Remove units from the user selection window that can not be selected (anymore) -> dead units
    std::erase_if(m_selectedObjects, [](auto& pObject) {
        return !pObject->isSelectable();
        });


    g_pRessourceManager->getRuneAnimation().process(); // Same animation/frame for all runes in parallel
    gSpellExecutor.process();

 
    // All objects must be removed here!
    pMap->removeObjects();

    // To not remove any objects inside of the following functions as they will 
    // invalidate the loops iterating over the players object vectors
    pMap->processMapObjects();
    pMap->processPlayers();   
    pMap->renderMiniMapOverlay(&gViewport); // ToDo: Maybe only each n'th tick?

    auto end = std::chrono::high_resolution_clock::now();
    float diff = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());


    avg += diff;
    ++cnt;
}


void Game::doRendering() {
    // imgui last on top
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style

    g_pRenderer->clear();


    // Start the Dear ImGui frame
    //ImGui_ImplSDL2Renderer_NewFrame();
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();



    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);


    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }



    wDebug.draw();
    wPlayer.draw();
    wMenuBar.draw();

    wMusicPlayer.draw();
    wPUDBrowser.draw();
    if (wPUDBrowser.selectedMap().size()) {
        m_quitRequested = true;
    }

    m_pMiniMap->draw();

 
    m_infoWindow.draw();

    //ifdebug
    gSpellWindow.draw();

    ImGui::ShowMetricsWindow();

    // Rendering (ImGui data is afterwards ready to be drawn on top)
    ImGui::Render();



    pMap->draw(g_pRenderer, &gViewport);


    const std::vector<Runes>& vecRunes = gSpellExecutor.getRunes();

    if (vecRunes.size()) {
        for (const Runes& r : vecRunes) {
            for (const Tile* pTile : r.vecRuneTiles) {
                if (!gViewport.isTileVisible(pTile))
                    continue;

                Animation& runeAnim = g_pRessourceManager->getRuneAnimation();
                Point posLocal_tile = gViewport.worldToScreen_tile(Point{ pTile->getX(), pTile->getY() });
                g_pRenderer->renderSprite(gViewport.tileToPixel(posLocal_tile), runeAnim.getCurrentFrame());
            }
        }

    }

    debugRenderAStar();


    // Draw selected units
    for (Object* pObject : m_selectedObjects) {
        if (!gViewport.isObjectVisible(pObject))
            continue;

        // Hier w\E4re methode OHNE offset notwendig! also globalToLocal doch auftrennen
        Point posLocal_px = gViewport.tileToPixel(gViewport.worldToScreen_tile(pObject->getTileLocation()));
        int tilesize = gMapUnitTilesize[typeid(*pObject)];
        
        // Sprite offsets are just non-zero when the object is moving from one tile to another
        Rect r = { posLocal_px.x + pObject->getTilePositionOffset().x, posLocal_px.y + pObject->getTilePositionOffset().y, tilesize * Tile::TileWidth_px + 1, tilesize * Tile::TileWidth_px + 1 };
        g_pRenderer->drawRect(r, 0, 255, 0);
    }


    if (rectDraggingActive) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        Rect r = { posLeftClicked.x, posLeftClicked.y, mouse_x - posLeftClicked.x, mouse_y - posLeftClicked.y };
        g_pRenderer->drawRect(r, 0, 255, 0);
    }


    // Last step: imgui on top
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    g_pRenderer->present();
}


// AStar debug (not optimized for performance -> not visible tiles are also rendered)
void Game::debugRenderAStar() {  
#ifdef _DEBUG
    Rect r = { 0,0,31,31 };

    // Closed List
    if (gDebugManager.m_astar_showClosedList) {        
        for (auto& pNode : g_debugSetClosedList) {
            r.x = (pNode->getX() - gViewport.getCurrentPosition_TL_tile().x) * Tile::TileWidth_px;
            r.y = (pNode->getY() - gViewport.getCurrentPosition_TL_tile().y) * Tile::TileWidth_px;
            g_pRenderer->drawRect(r, 255, 0, 0);
        }
    }

    // Open List
    if (gDebugManager.m_astar_showOpenList) {        
        for (auto& pNode : g_debugSetOpenList) {
            r.x = (pNode->getX() - gViewport.getCurrentPosition_TL_tile().x) * Tile::TileWidth_px;
            r.y = (pNode->getY() - gViewport.getCurrentPosition_TL_tile().y) * Tile::TileWidth_px;   
            g_pRenderer->drawRect(r, 0, 0, 255);
        }
    }

    // Actual Path
    if (gDebugManager.m_astar_showPath) {
        for (auto& pNode : g_debugPath.m_vecPathNodes) {
            r.x = (pNode->getX() - gViewport.getCurrentPosition_TL_tile().x) * Tile::TileWidth_px;
            r.y = (pNode->getY() - gViewport.getCurrentPosition_TL_tile().y) * Tile::TileWidth_px;
            g_pRenderer->drawRect(r, 0, 255, 0);
        }
    }
#endif
}
