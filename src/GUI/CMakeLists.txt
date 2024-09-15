# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------
SET(SRC_LIST
    ${SRC_LIST}  
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugSpellWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugSpellWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HelperFunctions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HelperFunctions.h

    ${CMAKE_CURRENT_SOURCE_DIR}/MenuBarWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MenuBarWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MiniMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MiniMap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MusicPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MusicPlayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ObjectInfoWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObjectInfoWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayerStatsWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayerStatsWindow.h


    ${CMAKE_CURRENT_SOURCE_DIR}/PUDBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PUDBrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Window.h

    # imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h


# imgui Variant 1: Using SDLRenderer (less flexibility)
# Allows software rendering with virtual box. Requires SDL 2.0.17
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdlrenderer2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdlrenderer2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdl2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdl2.h  


# imgui Variant 2: SDL with OpenGL (Currently not used)
# Both backends are used together. See imgui/examples/example_sdl_opengl3
# When just using impl_sdl there is no way to render draw data (ImGui_ImplOpenGL3_RenderDrawData)
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3_loader.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.h        
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdl.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdl.h    
#
#
#    # gl3w
#    /usr/local/share/gl3w/gl3w.c


    PARENT_SCOPE)
