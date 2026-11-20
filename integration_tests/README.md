# War25 Integration Tests

This directory contains comprehensive integration tests for the War25 project, focusing on component interactions and system workflows.

## Overview

Integration tests verify that different components of the War25 system work correctly together. Unlike unit tests that test individual components in isolation, integration tests ensure that the interfaces between components function as expected and that complex workflows operate correctly.

## Test Structure

```
integration_tests/
├── CMakeLists.txt              # CMake configuration for integration tests
├── README.md                   # This documentation file
├── main.cpp                    # Integration test runner entry point
├── MockFramework.h             # Mock SDL and framework dependencies
├── SimpleIntegrationTest.cpp   # Basic component integration tests
├── MapTileIntegrationTest.cpp  # Map and tile system integration tests
├── PathfindingIntegrationTest.cpp # A* pathfinding algorithm integration tests
└── CombatIntegrationTest.cpp   # Combat system integration tests
```

## Integration Test Categories

### 1. **SimpleIntegrationTest** - Foundation Integration
Tests basic component interactions and framework functionality:

- **Resource Management**: Resource creation, manipulation, and operations
- **Player Properties**: Player colors, fractions, and basic properties
- **Coordinate Systems**: Point operations and coordinate transformations
- **Graphics Systems**: Mock texture and sprite management
- **Animation Systems**: Frame processing and animation workflows
- **Utility Functions**: Helper functions and data structure operations
- **Performance Testing**: Multi-operation performance validation
- **Error Handling**: Edge cases and error conditions

### 2. **MapTileIntegrationTest** - World System Integration
Tests map and tile system interactions:

- **Map Properties**: Map dimensions, tile access, and boundary checking
- **Terrain Features**: Water, land, coast, and forest terrain interactions
- **Neighbor Discovery**: Adjacent tile finding and distance calculations
- **Unit Placement**: Single and multi-tile unit placement on maps
- **Collision Detection**: Object blocking and space availability
- **Layer Management**: Land/sea units vs air units on separate layers
- **Terrain Constraints**: Building and unit placement restrictions

### 3. **PathfindingIntegrationTest** - Navigation System Integration
Tests A* pathfinding algorithm with map integration:

- **Basic Pathfinding**: Simple point-to-point navigation
- **Obstacle Avoidance**: Navigation around blocked tiles
- **Optimal Paths**: Shortest path calculation verification
- **Edge Cases**: Same start/goal, invalid positions, no path scenarios
- **Complex Navigation**: L-shaped obstacles, maze solving
- **Performance**: Multiple pathfinding requests and state management
- **Algorithm Correctness**: Path continuity and cost optimization

### 4. **CombatIntegrationTest** - Battle System Integration
Tests combat mechanics and unit interactions:

- **Basic Combat**: Attack/defense mechanics between units
- **Damage Calculation**: Basic damage, piercing damage, and armor interactions
- **Unit Health**: Health management, healing, and death mechanics
- **Special Abilities**: Magic spells, upgrades, and unit-specific powers
- **Group Combat**: Multi-unit battle scenarios
- **Unit Types**: Different unit characteristics and combat roles
- **Range Mechanics**: Melee vs ranged combat advantages

## Key Features

### Mock Framework Integration
- **SDL Independence**: Tests run without requiring SDL graphics libraries
- **Resource Management**: Mock texture and sprite systems
- **Type Safety**: Proper C++ type system with enum classes
- **Memory Management**: RAII and smart pointer usage

### Real Algorithm Testing
- **A* Implementation**: Full pathfinding algorithm with heuristics
- **Combat Formulas**: Accurate damage calculation with armor
- **Geometric Operations**: Distance calculations and coordinate transformations
- **State Management**: Complex object state transitions

### Performance Validation
- **Batch Operations**: Testing multiple simultaneous operations
- **Memory Efficiency**: Resource allocation and deallocation patterns
- **Algorithm Complexity**: Pathfinding performance with large maps

## Building and Running

### Prerequisites
- CMake 3.20 or later
- C++20 compatible compiler (GCC 13.3+ recommended)
- Internet connection (for downloading Google Test)

### Build Instructions

1. **Navigate to integration tests directory:**
   ```bash
   cd war25/integration_tests
   ```

2. **Create build directory:**
   ```bash
   mkdir build && cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the tests:**
   ```bash
   make -j4
   ```

5. **Run the integration tests:**
   ```bash
   ./war25_integration_tests
   ```

### Expected Output

The integration tests will show:
- **Total Test Count**: Number of integration scenarios tested
- **Pass/Fail Results**: Individual test outcomes
- **Failed Test Details**: Specific integration issues discovered
- **Performance Metrics**: Execution time for complex scenarios

## Test Results Analysis

### Current Test Status (Example Run)
- **Total Tests**: 45 integration scenarios
- **Passed**: 39 tests (86.7% success rate)
- **Failed**: 6 tests revealing integration issues

### Common Integration Issues Detected
1. **Combat Mechanics**: Damage calculation edge cases
2. **Map Placement**: Terrain constraint violations
3. **Pathfinding**: Start/goal position handling
4. **Resource Management**: Transaction boundary conditions

## Test Design Philosophy

### Integration vs Unit Testing
- **Unit Tests**: Test individual classes and functions in isolation
- **Integration Tests**: Test component interactions and workflows
- **System Tests**: Test complete game scenarios (future work)

### Mock Strategy
- **Minimal Mocking**: Only mock external dependencies (SDL, file I/O)
- **Real Algorithms**: Use actual game logic implementations
- **Controlled Environment**: Predictable test conditions
- **Reproducible Results**: Deterministic test outcomes

## Contributing to Integration Tests

### Adding New Integration Tests

1. **Identify Integration Points**: Find components that interact
2. **Create Test Scenarios**: Design realistic usage patterns
3. **Use Mock Framework**: Leverage existing mock infrastructure
4. **Test Error Conditions**: Include failure scenarios
5. **Document Test Purpose**: Clear test names and comments

### Example Test Structure
```cpp
TEST_F(YourIntegrationTest, ComponentInteractionScenario) {
    // Arrange: Setup test environment
    auto component1 = createComponent1();
    auto component2 = createComponent2();

    // Act: Perform integration operation
    auto result = component1->interactWith(component2);

    // Assert: Verify integration behavior
    EXPECT_TRUE(result.isSuccessful());
    EXPECT_EQ(component1->getState(), ExpectedState::Updated);
}
```

### Best Practices

1. **Test Real Workflows**: Focus on actual usage patterns
2. **Verify State Changes**: Check all affected components
3. **Include Edge Cases**: Test boundary conditions
4. **Performance Awareness**: Monitor execution time
5. **Clear Naming**: Descriptive test and variable names

## Integration with CI/CD

The integration test suite is designed for continuous integration:

- **Fast Execution**: All tests complete in ~1 second
- **Deterministic Results**: No random or time-dependent behavior
- **Clear Output**: Structured pass/fail reporting
- **Exit Codes**: Standard success/failure codes for automation

### CI Configuration Example
```yaml
integration_tests:
  runs-on: ubuntu-latest
  steps:
    - name: Build Integration Tests
      run: |
        cd integration_tests/build
        cmake ..
        make -j4
    - name: Run Integration Tests
      run: ./war25_integration_tests
```

## Future Enhancements

### Planned Integration Test Areas

1. **Building Construction**: Multi-stage building workflows
2. **Resource Chains**: Mining → Processing → Trading workflows
3. **Technology Trees**: Upgrade dependencies and unlocks
4. **Multiplayer Interactions**: Player-to-player system integration
5. **Save/Load Systems**: Game state persistence integration
6. **AI Behaviors**: AI decision-making with game systems

### Advanced Testing Techniques

1. **Property-Based Testing**: Generate random valid scenarios
2. **Mutation Testing**: Verify test effectiveness
3. **Performance Profiling**: Identify integration bottlenecks
4. **Stress Testing**: High-load integration scenarios

## Troubleshooting

### Common Build Issues

**CMake Configuration Fails:**
- Verify CMake version (3.20+)
- Check internet connection for GoogleTest download
- Ensure C++20 compiler support

**Compilation Errors:**
- Check include paths in CMakeLists.txt
- Verify MockFramework.h compatibility
- Update compiler if C++20 features missing

**Runtime Failures:**
- Review test output for specific failure details
- Check mock object initialization
- Verify test data validity

### Debug Mode

Run tests with verbose output:
```bash
./war25_integration_tests --gtest_verbose
```

Filter specific test categories:
```bash
./war25_integration_tests --gtest_filter="*Combat*"
```

## Related Documentation

- **Unit Tests**: `/tests/README.md` - Component-level testing
- **Main Project**: `/README.md` - Overall project documentation
- **Build System**: `/src/CMakeLists.txt` - Main build configuration
- **Testing Guide**: `/TESTING.md` - Complete testing overview