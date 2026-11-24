# Test CMakeLists.txt for war25
cmake_minimum_required(VERSION 3.20)
project(war25_tests LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading GoogleTest
include(FetchContent)

# Download and configure GoogleTest with a stable release
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.14.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Collect test source files
file(GLOB TEST_SOURCES
    "*.cpp"
    "*.h"
)

# Create test executable
add_executable(
    war25_tests
    ${TEST_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/EventHandling/EventBroker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/EventHandling/EventProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Global/PseudoRandom.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Magic/IMagical.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Network/LocalNetworkSession.cpp
)

# Include directories for war25 source code
target_include_directories(war25_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Buildings
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/EventHandling
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Global
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Magic
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Map
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Units
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Resources
)

# Create a minimal interface library for basic includes
add_library(war25_test_common INTERFACE)
target_include_directories(war25_test_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Link against gtest and minimal dependencies
target_link_libraries(
    war25_tests
    PRIVATE
    gtest
    gmock
    war25_test_common
)

# Set C++20 standard for tests
target_compile_features(war25_tests PRIVATE cxx_std_20)

if(WIN32)
    target_link_libraries(war25_tests PRIVATE ws2_32)
endif()

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(war25_tests)
