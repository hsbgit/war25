# -----------------------------------------------------------------------------
# war25 - Modern CMake Implementation
# This project is licensed under the GNU General Public License v3.0.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
project(war25 LANGUAGES CXX)

# Modern C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Create interface library for common settings
add_library(war25_common INTERFACE)

# Platform-specific dependency configuration
if(WIN32)
    # Windows dependencies (bundled)
    set(WAR25_SDL2_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2-2.30.0")
    set(WAR25_SDL2_IMAGE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2_image-2.8.2")
    set(WAR25_SDL2_MIXER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2_mixer-2.8.0")
    set(WAR25_WAR2TOOLS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/war2tools")

    target_include_directories(war25_common INTERFACE
        "${WAR25_SDL2_ROOT}/include"
        "${WAR25_SDL2_IMAGE_ROOT}/include"
        "${WAR25_SDL2_MIXER_ROOT}/include"
        "${WAR25_WAR2TOOLS_ROOT}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_link_libraries(war25_common INTERFACE
        "${WAR25_SDL2_ROOT}/lib/x64/SDL2.lib"
        "${WAR25_SDL2_IMAGE_ROOT}/lib/x64/SDL2_image.lib"
        "${WAR25_SDL2_MIXER_ROOT}/lib/x64/SDL2_mixer.lib"
        "${WAR25_WAR2TOOLS_ROOT}/lib/libpud.lib"
        "${WAR25_WAR2TOOLS_ROOT}/lib/libwar2.lib"
        Winmm.lib
    )

else()
    # Linux/Unix dependencies (system packages)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    find_package(PkgConfig REQUIRED)

    # Use pkg-config for SDL2 libraries when possible
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE sdl2_image)
    pkg_check_modules(SDL2_MIXER sdl2_mixer)

    target_include_directories(war25_common INTERFACE
        ${SDL2_INCLUDE_DIRS}
        "/usr/local/include/gl3w"
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    # Fallback for missing system packages - use older style libraries
    set(SDL2_LIBS ${SDL2_LIBRARIES})
    if(NOT SDL2_IMAGE_FOUND)
        list(APPEND SDL2_LIBS "SDL2_image")
    else()
        list(APPEND SDL2_LIBS ${SDL2_IMAGE_LIBRARIES})
    endif()

    if(NOT SDL2_MIXER_FOUND)
        list(APPEND SDL2_LIBS "SDL2_mixer")
    else()
        list(APPEND SDL2_LIBS ${SDL2_MIXER_LIBRARIES})
    endif()

    target_link_libraries(war25_common INTERFACE
        ${SDL2_LIBS}
        "/usr/local/lib/libpud.so"
        "/usr/local/lib/libwar2.so"
        "/usr/local/lib/libglfw3.a"
        Threads::Threads
        dl
        GL
    )

    target_compile_options(war25_common INTERFACE ${SDL2_CFLAGS_OTHER})
endif()

# Component libraries
add_subdirectory(Buildings)
add_subdirectory(EventHandling)
add_subdirectory(Global)
add_subdirectory(GUI)
add_subdirectory(Magic)
add_subdirectory(Map)
add_subdirectory(Units)
add_subdirectory(Resources)

# Main executable sources (only root level files)
file(GLOB MAIN_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Filter out any CMake build files that might have been included
list(FILTER MAIN_SOURCES EXCLUDE REGEX ".*CMakeFiles.*")
list(FILTER MAIN_SOURCES EXCLUDE REGEX ".*cmake-build.*")
list(FILTER MAIN_SOURCES EXCLUDE REGEX ".*build.*")

# Create main executable with all object libraries
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCES}
    $<TARGET_OBJECTS:war25_buildings>
    $<TARGET_OBJECTS:war25_eventhandling>
    $<TARGET_OBJECTS:war25_global>
    $<TARGET_OBJECTS:war25_gui>
    $<TARGET_OBJECTS:war25_magic>
    $<TARGET_OBJECTS:war25_map>
    $<TARGET_OBJECTS:war25_units>
    $<TARGET_OBJECTS:war25_units_air>
    $<TARGET_OBJECTS:war25_units_land>
    $<TARGET_OBJECTS:war25_units_sea>
    $<TARGET_OBJECTS:war25_units_resource>
    $<TARGET_OBJECTS:war25_units_dwarfes>
    $<TARGET_OBJECTS:war25_units_peasant>
    $<TARGET_OBJECTS:war25_peasant_chopwood>
    $<TARGET_OBJECTS:war25_peasant_constructbuilding>
    $<TARGET_OBJECTS:war25_peasant_mining>
    $<TARGET_OBJECTS:war25_resources>
)

# Link only the external dependencies (no internal libraries needed anymore)
target_link_libraries(${PROJECT_NAME} PRIVATE war25_common)

# Platform-specific compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)
