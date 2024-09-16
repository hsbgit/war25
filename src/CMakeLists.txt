# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# !!! ToDo: This file needs a big refactoring !!!
# ----------------------------------------------------------------------------

SET(project_name war25) 
project(${project_name})
cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_CXX_STANDARD 20)

# Enable debug if required
set (CMAKE_BUILD_TYPE  Debug)


#set(Boost_USE_STATIC_LIBS OFF)
#find_package(Boost COMPONENTS *boost libraries here*)


if (WIN32)
# C++ standard is set as a command line option in visual studio
# https://stackoverflow.com/questions/41308933/how-to-enable-c17-compiling-in-visual-studio 

# SDL2
SET(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2-2.30.0/include)
#SET(SDL2_DIRECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2/include/SDL2)
SET(SDL2_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2-2.30.0/lib/x64/SDL2.lib)

# SDL_image
SET(SDL_IMAGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2_image-2.8.2/include)
SET(SDL_IMAGE_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2_image-2.8.2/lib/x64/SDL2_image.lib)

# SDL_mixer
SET(SDL_MIXER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2_mixer-2.8.0/include)
SET(SDL2_MIXER_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/SDL2_mixer-2.8.0/lib/x64/SDL2_mixer.lib)
#SET(SDL2_MIXER_TIMIDITY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL2/lib/timidity.lib)
#SET(SDL2_MIXER_NATIVE_MIDI_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL2/lib/native_midi.lib) # Requires Winmm.lib

# war2tools
SET(WAR2TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/war2tools/include)
SET(LIBPUD_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/war2tools/lib/libpud.lib)
SET(LIBWAR2_LIB ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/win/war2tools/lib/libwar2.lib)

else ()
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)
find_package(glfw3 REQUIRED)

SET(SDL2_INCLUDE_DIR /usr/local/include/SDL2/)
include_directories(${SDL2_INCLUDE_DIRS})
SET(SDL_IMAGE_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so)
SET(SDL2_TTF SDL2_ttf)
SET(SDL2_MIXER_LIB SDL2_mixer)

SET(GL3W_INCLUDE_DIR /usr/local/include/gl3w/)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


# Headers (pud.h and war2.h) are copied to /usr/local/include during installation, no action required here
SET(LIBPUD_LIB /usr/local/lib/libpud.so)
SET(LIBWAR2_LIB /usr/local/lib/libwar2.so)

SET(GLFW_LIB /usr/local/lib/libglfw3.a)

endif()






# The last '.' is important to allow proper header file including, e.g., #include <Global/Object.h>
include_directories(
${SDL2_INCLUDE_DIR}
${SDL2_DIRECT_INCLUDE_DIR}
${SDL_IMAGE_INCLUDE}
${SDL_MIXER_INCLUDE}
${GL3W_INCLUDE_DIR}
#${GLFW_INCLUDE_DIR}
#${PNG_INCLUDE_DIR}
#${PNG_INCLUDES_DIR}
#${ZLIB_INCLUDE_DIR}
${WAR2TOOLS_INCLUDE_DIR}
.)

# Stackoverflow: add source in a subdirectory to a cmake project
add_subdirectory(Buildings)
add_subdirectory(EventHandling)
add_subdirectory(Global)
add_subdirectory(GUI)
add_subdirectory(Magic)
add_subdirectory(Map)
add_subdirectory(Units)
add_subdirectory(Resources)



aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})


# -Wall -Wextra -pedantic -Werror
# add_compile_options(-j2) not working
#target_compile_options(${PROJECT_NAME} PRIVATE "-j 2"")
#SET(CMAKE_CXX_FLAGS "-j 2")



target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL_IMAGE_LIBRARY} ${SDL2_TTF} ${SDL2_MIXER_LIB} ${SDL2_MIXER_TIMIDITY_LIB} ${SDL2_MIXER_NATIVE_MIDI_LIB} ${LIBWAR2_LIB} ${LIBPUD_LIB})
if (WIN32)
# Winmm.lib is required by ${SDL2_MIXER_NATIVE_MIDI_LIB}
target_link_libraries(${PROJECT_NAME} Winmm.lib)
else ()
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${GLFW_LIB} glfw -lpthread dl GL)
endif()

#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL_IMAGE_LIBRARY} ${SDL2_TTF} ${SDL2_MIXER_LIB} ${LIBWAR2_LIB} ${LIBPUD_LIB} ${GLFW_LIB} dl GL) #  ${PNG_LIBRARIES}   ${ZLIB_LIBRARY}

# only linux...
#target_link_libraries(${project_name} -lpthread)

