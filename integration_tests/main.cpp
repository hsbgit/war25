#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "War25 Integration Tests Starting..." << std::endl;
    std::cout << "Integration tests verify component interactions and system workflows." << std::endl;

    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Running all integration tests..." << std::endl;

    int result = RUN_ALL_TESTS();

    if (result == 0) {
        std::cout << "All integration tests passed successfully!" << std::endl;
        std::cout << "Components are properly integrated and workflows function correctly." << std::endl;
    } else {
        std::cout << "Some integration tests failed. Check output above for details." << std::endl;
        std::cout << "Integration failures indicate issues with component interactions." << std::endl;
    }

    return result;
}