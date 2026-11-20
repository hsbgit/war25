# War25 Unit Tests

This directory contains comprehensive unit tests for the War25 project using Google Test (GTest) framework.

## Overview

The test suite covers the following areas:

- **Basic functionality tests** - Framework validation and fundamental C++ operations
- **Game logic tests** - Core gameplay mechanics like health, damage, resources
- **Data structure tests** - Point operations, collections, and algorithms
- **Enum validation** - Player colors, fractions, directions, and action types
- **Pathfinding concepts** - Basic A* algorithm components
- **Unit and building logic** - Combat stats, buffs, and abilities

## Structure

```
tests/
├── CMakeLists.txt          # CMake configuration for tests
├── README.md              # This file
├── main.cpp               # Test main entry point
├── BasicTest.cpp          # Basic framework and C++ functionality tests
└── StandaloneTest.cpp     # Standalone game logic tests (no SDL dependencies)
```

## Building and Running Tests

### Prerequisites

- CMake 3.20 or later
- C++20 compatible compiler (GCC 13.3+ recommended)
- Internet connection (for downloading Google Test)

### Build Instructions

1. Navigate to the tests directory:
   ```bash
   cd war25/tests
   ```

2. Create a build directory:
   ```bash
   mkdir build && cd build
   ```

3. Configure with CMake:
   ```bash
   cmake ..
   ```

4. Build the tests:
   ```bash
   make -j4
   ```

5. Run the tests:
   ```bash
   ./war25_tests
   ```

### Integration with Main Project

Tests can also be built from the main project:

1. From the `src` directory:
   ```bash
   cd src/build
   cmake .. -DBUILD_TESTING=ON
   make -j4
   ```

2. Run tests:
   ```bash
   cd tests
   ./war25_tests
   ```

## Test Categories

### BasicTest
- Framework functionality validation
- String and mathematical operations
- Boolean logic
- Type system validation

### War25StandaloneTest
- Game-specific functionality
- Point and coordinate system
- Direction and movement enums
- Player colors and fractions
- Combat statistics (AttackerStats)
- Action enums for different unit types
- Buff system mechanics
- Pathfinding algorithm concepts
- Data structure operations
- Health and damage calculations
- Resource management

## Design Philosophy

The tests are designed to be:

1. **Independent** - No dependencies on SDL, OpenGL, or other external game libraries
2. **Fast** - All tests run in milliseconds
3. **Comprehensive** - Cover core game mechanics and data structures
4. **Maintainable** - Clear test names and well-organized structure
5. **Extensible** - Easy to add new tests for additional functionality

## Test Coverage

Current test coverage includes:

- ✅ Basic data types and structures
- ✅ Game enumerations (Direction, PlayerColor, Fraction)
- ✅ Action systems (GeneralActions, PeasantActions, MageActions, etc.)
- ✅ Combat statistics and damage calculation
- ✅ Buff system mechanics
- ✅ Pathfinding algorithm concepts
- ✅ Resource management
- ✅ Health calculation systems

### Areas for Future Expansion

When SDL and other dependencies can be integrated, consider adding:

- Complete Map and Tile functionality tests
- Full Unit class hierarchies
- AStar pathfinding with real nodes
- Building construction and upgrade mechanics
- Magic system and spell casting
- Event handling and state machines
- Audio and rendering system tests

## Adding New Tests

To add new tests:

1. Create a new test file or add to existing ones
2. Follow the existing naming conventions
3. Use descriptive test names that explain what is being tested
4. Group related tests into test fixtures
5. Ensure tests are independent and can run in any order

Example test structure:
```cpp
class YourTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
};

TEST_F(YourTestFixture, DescriptiveTestName) {
    // Test implementation
    EXPECT_EQ(expected, actual);
}
```

## Continuous Integration

The test suite is designed to be easily integrated into CI/CD pipelines:

- All dependencies are automatically downloaded via CMake FetchContent
- No external package installations required
- Fast execution time suitable for frequent runs
- Clear pass/fail output suitable for automated systems

## Contributing

When contributing new game features, please:

1. Add corresponding unit tests
2. Ensure all existing tests pass
3. Follow the established testing patterns
4. Document any new test utilities or helpers
5. Update this README if new test categories are added

## Troubleshooting

### Common Issues

**Build fails with missing headers**: Ensure you're building from the correct directory and that all paths in CMakeLists.txt are correct.

**Google Test download fails**: Check internet connection and try cleaning the build directory.

**Tests fail unexpectedly**: Run tests with verbose output: `./war25_tests --gtest_verbose`

### Getting Help

For testing-related issues:
1. Check the CMake configuration
2. Verify C++ standard support
3. Ensure all dependencies are properly linked
4. Check test logs for detailed error messages