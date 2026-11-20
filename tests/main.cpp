#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "War25 Unit Tests Starting..." << std::endl;

    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Running all tests..." << std::endl;

    int result = RUN_ALL_TESTS();

    if (result == 0) {
        std::cout << "All tests passed successfully!" << std::endl;
    } else {
        std::cout << "Some tests failed. Check output above." << std::endl;
    }

    return result;
}