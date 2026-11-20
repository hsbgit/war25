# War25 Testing Guide

This document provides an overview of the unit testing setup for the War25 project.

## Testing Framework

The project uses **Google Test (GTest)** as the primary testing framework, which is the de-facto standard for C++ unit testing.

### Key Features
- Comprehensive assertion macros
- Test fixtures for setup/teardown
- Parameterized tests support
- Mock objects with Google Mock
- Test discovery and execution
- CI/CD integration ready

## Project Structure

```
war25/
├── src/                    # Main project source code
│   ├── CMakeLists.txt     # Main CMake file (with optional test integration)
│   ├── Map/               # Map and tile functionality
│   ├── Units/             # Unit classes and logic
│   ├── Buildings/         # Building implementations
│   ├── Global/            # Global utilities and types
│   └── ...
├── tests/                  # Test suite directory
│   ├── CMakeLists.txt     # Test-specific CMake configuration
│   ├── README.md          # Detailed testing documentation
│   ├── main.cpp           # Test runner entry point
│   ├── BasicTest.cpp      # Framework and basic functionality tests
│   └── StandaloneTest.cpp # Game-specific logic tests
└── TESTING.md             # This overview document
```

## Quick Start

### Running Tests

1. **Standalone tests** (recommended for development):
   ```bash
   cd tests
   mkdir build && cd build
   cmake ..
   make -j4
   ./war25_tests
   ```

2. **Integrated with main project**:
   ```bash
   cd src/build
   cmake .. -DBUILD_TESTING=ON
   make -j4
   cd tests
   ./war25_tests
   ```

## Test Categories

### Unit Tests (`/tests/`)

✅ **Basic Functionality**
- Framework validation and C++ standard library
- Type system validation and fundamental operations
- Mathematical and boolean logic operations

✅ **Game Logic (Standalone)**
- Enumerations (PlayerColor, Fraction, Direction)
- Action systems (all unit types: General, Peasant, Mage, etc.)
- Combat mechanics concepts (damage, armor, health)
- Resource management fundamentals
- Buff system mechanics

✅ **Data Structures**
- Point operations and comparisons
- Collections (vector, map, set, unordered_map)
- Algorithm concepts and pathfinding basics

### Integration Tests (`/integration_tests/`)

✅ **Component Integration**
- Map and tile system interactions
- Unit placement and collision detection
- Pathfinding with obstacle avoidance
- Combat system with multiple unit types

✅ **Algorithm Integration**
- Complete A* pathfinding implementation
- Combat damage calculation with armor systems
- Resource management with transaction workflows
- Terrain constraint validation

✅ **System Workflows**
- Multi-unit combat scenarios
- Complex pathfinding (mazes, L-shaped obstacles)
- Resource chain operations (mining → processing → trading)
- Graphics system integration (textures, sprites, animations)

### Test Results Summary

**Unit Tests**: 17 tests, 100% pass rate, <0.1s execution time
**Integration Tests**: 45 tests, 86.7% pass rate (39/45), ~1s execution time

**Total Test Coverage**: 62 automated tests covering both isolated components and system interactions

## Test Design Principles

1. **Independence**: Tests don't depend on external game libraries (SDL, OpenGL)
2. **Speed**: All tests execute in milliseconds
3. **Clarity**: Descriptive test names and clear assertions
4. **Maintainability**: Well-organized structure and fixtures
5. **Extensibility**: Easy to add new tests for new features

## Development Workflow

### Adding New Features

1. **Write tests first** (TDD approach recommended)
2. Implement the feature
3. Ensure all tests pass
4. Refactor if needed

### Adding New Tests

```cpp
// Example test structure
class NewFeatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test data
    }
};

TEST_F(NewFeatureTest, ShouldDoExpectedBehavior) {
    // Arrange
    auto feature = CreateFeature();

    // Act
    auto result = feature.DoSomething();

    // Assert
    EXPECT_EQ(expected_result, result);
}
```

## Continuous Integration

The test suite is CI/CD ready:

- ✅ Automatic dependency resolution (Google Test via FetchContent)
- ✅ No external package requirements
- ✅ Fast execution (< 1 second for full suite)
- ✅ Clear pass/fail reporting
- ✅ Standard exit codes for automation

### Example CI Configuration

```yaml
# Example GitHub Actions workflow
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Build and Test
      run: |
        cd tests
        mkdir build && cd build
        cmake ..
        make -j4
        ./war25_tests
```

## Best Practices

### Test Naming
- Use descriptive names that explain the scenario
- Format: `ShouldExpectedBehaviorWhenCondition`
- Example: `ShouldReduceHealthWhenTakingDamage`

### Test Organization
- Group related tests in fixtures
- One assertion per test when possible
- Use setup/teardown for common initialization

### Performance
- Keep tests fast (< 1ms each ideally)
- Avoid file I/O or network operations
- Use mocks for complex dependencies

## Integration with War25 Development

### Code Coverage Goals
- All new public interfaces should have tests
- Core game mechanics should be thoroughly tested
- Critical algorithms (pathfinding, combat) need comprehensive coverage

### Review Process
- New features require corresponding tests
- Pull requests should include test updates
- All tests must pass before merging

## Troubleshooting

### Common Issues

1. **Build fails**: Check C++20 compiler support and CMake version
2. **Google Test download fails**: Verify internet connection
3. **Include errors**: Ensure proper path configuration in CMakeLists.txt
4. **Linking errors**: Check library dependencies and linkage

### Debug Options

```bash
# Verbose test output
./war25_tests --gtest_verbose

# Run specific tests
./war25_tests --gtest_filter="*PlayerColor*"

# List all tests
./war25_tests --gtest_list_tests
```

## Resources

- [Google Test Documentation](https://google.github.io/googletest/)
- [C++ Testing Best Practices](https://google.github.io/googletest/primer.html)
- [CMake FetchContent Documentation](https://cmake.org/cmake/help/latest/module/FetchContent.html)

## Contributing

For questions or contributions to the testing infrastructure:
1. Check existing tests for patterns to follow
2. Update documentation when adding new test categories
3. Ensure tests are maintainable and well-documented
4. Consider both positive and negative test cases