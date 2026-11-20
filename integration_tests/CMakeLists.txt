# Integration Tests CMakeLists.txt for war25
cmake_minimum_required(VERSION 3.20)
project(war25_integration_tests LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading GoogleTest
include(FetchContent)

# Download and configure GoogleTest with a stable release
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.14.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Collect integration test source files
file(GLOB INTEGRATION_TEST_SOURCES
    "*.cpp"
    "*.h"
)

# Create integration test executable
add_executable(
    war25_integration_tests
    ${INTEGRATION_TEST_SOURCES}
)

# Include directories for war25 source code
target_include_directories(war25_integration_tests PRIVATE
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

# Create integration test framework
add_library(war25_integration_common INTERFACE)

# Mock SDL types for integration testing
target_compile_definitions(war25_integration_tests PRIVATE
    -DINTEGRATION_TEST_MODE=1
)

# Link against gtest and minimal dependencies
target_link_libraries(
    war25_integration_tests
    PRIVATE
    gtest
    gtest_main
    gmock
    gmock_main
    war25_integration_common
)

# Set C++20 standard for tests
target_compile_features(war25_integration_tests PRIVATE cxx_std_20)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(war25_integration_tests)