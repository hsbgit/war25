# war25 GUI component library
set(GUI_SOURCES
    # Main GUI components
    DebugSpellWindow.cpp
    DebugSpellWindow.h
    DebugWindow.cpp
    DebugWindow.h
    HelperFunctions.cpp
    HelperFunctions.h
    MenuBarWindow.cpp
    MenuBarWindow.h
    MiniMap.cpp
    MiniMap.h
    MusicPlayer.cpp
    MusicPlayer.h
    ObjectInfoWindow.cpp
    ObjectInfoWindow.h
    PlayerStatsWindow.cpp
    PlayerStatsWindow.h
    PUDBrowser.cpp
    PUDBrowser.h
    Window.cpp
    Window.h
)

# ImGui core sources
set(IMGUI_CORE_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

# ImGui headers (for installation/interface)
set(IMGUI_HEADERS
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
)

# ImGui SDL backend (active variant)
set(IMGUI_SDL_SOURCES
    imgui/imgui_impl_sdlrenderer2.cpp
    imgui/imgui_impl_sdlrenderer2.h
    imgui/imgui_impl_sdl2.cpp
    imgui/imgui_impl_sdl2.h
)

# Create the library
add_library(war25_gui OBJECT
    ${GUI_SOURCES}
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_HEADERS}
    ${IMGUI_SDL_SOURCES}
)

target_link_libraries(war25_gui PUBLIC war25_common)

# ImGui-specific include directory for the imgui subdirectory
target_include_directories(war25_gui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
)
