#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockFramework.h"
#include <vector>
#include <memory>
#include <deque>
#include <algorithm>

// Simple integration test for war25 components
class SimpleIntegrationTest : public War25IntegrationTest {
protected:
    void SetUp() override {
        War25IntegrationTest::SetUp();
    }
};

// Test basic resource management integration
TEST_F(SimpleIntegrationTest, ResourcePlayerIntegration) {
    // Test resource creation and manipulation
    Resources playerResources(1000, 500, 100);

    EXPECT_EQ(playerResources.gold(), 1000);
    EXPECT_EQ(playerResources.lumber(), 500);
    EXPECT_EQ(playerResources.oil(), 100);

    // Test resource operations
    Resources cost(200, 100, 0);
    EXPECT_TRUE(playerResources >= cost);

    playerResources -= cost;
    EXPECT_EQ(playerResources.gold(), 800);
    EXPECT_EQ(playerResources.lumber(), 400);
    EXPECT_EQ(playerResources.oil(), 100);

    // Test resource addition
    Resources income(50, 25, 10);
    playerResources += income;
    EXPECT_EQ(playerResources.gold(), 850);
    EXPECT_EQ(playerResources.lumber(), 425);
    EXPECT_EQ(playerResources.oil(), 110);
}

// Test player color and fraction integration
TEST_F(SimpleIntegrationTest, PlayerPropertiesIntegration) {
    PlayerColor red = PlayerColor::Red;
    PlayerColor blue = PlayerColor::Blue;
    PlayerColor neutral = PlayerColor::Neutral;

    EXPECT_EQ(static_cast<int>(red), 0);
    EXPECT_EQ(static_cast<int>(blue), 1);
    EXPECT_EQ(static_cast<int>(neutral), 15);

    Fraction alliance = Fraction::Alliance;
    Fraction orc = Fraction::Orc;
    Fraction neutralFraction = Fraction::Neutral;

    EXPECT_NE(alliance, orc);
    EXPECT_NE(alliance, neutralFraction);
    EXPECT_NE(orc, neutralFraction);
}

// Test point and coordinate system integration
TEST_F(SimpleIntegrationTest, CoordinateSystemIntegration) {
    Point p1 = IntegrationTestUtils::createTestPoint(10, 20);
    Point p2 = IntegrationTestUtils::createTestPoint(10, 20);
    Point p3 = IntegrationTestUtils::createTestPoint(15, 25);

    EXPECT_TRUE(IntegrationTestUtils::pointsEqual(p1, p2));
    EXPECT_FALSE(IntegrationTestUtils::pointsEqual(p1, p3));

    // Test point operations
    EXPECT_EQ(p1.x, 10);
    EXPECT_EQ(p1.y, 20);
    EXPECT_TRUE(p1 == p2);
    EXPECT_TRUE(p1 < p3);
}

// Test mock texture and sprite system integration
TEST_F(SimpleIntegrationTest, GraphicsSystemIntegration) {
    auto texture = std::make_shared<MockTexture>(64, 64);
    EXPECT_EQ(texture->getWidth(), 64);
    EXPECT_EQ(texture->getHeight(), 64);

    MockSprite sprite(texture);
    EXPECT_EQ(sprite.getTexture(), texture.get());

    // Test resource manager
    auto textureName = "test_texture";
    auto retrievedTexture = mockResourceManager->getTexture(textureName);
    EXPECT_NE(retrievedTexture, nullptr);

    // Retrieving same texture should return same object
    auto sameTexture = mockResourceManager->getTexture(textureName);
    EXPECT_EQ(retrievedTexture, sameTexture);
}

// Test animation system integration
TEST_F(SimpleIntegrationTest, AnimationSystemIntegration) {
    MockAnimation singleFrameAnimation;
    MockAnimation multiFrameAnimation(5);

    // Single frame animation
    MockSprite frame1 = singleFrameAnimation.getCurrentFrame();
    singleFrameAnimation.process();
    MockSprite frame2 = singleFrameAnimation.getCurrentFrame();

    // Both should be valid
    EXPECT_NE(frame1.getTexture(), nullptr);
    EXPECT_NE(frame2.getTexture(), nullptr);

    // Multi-frame animation
    for (int i = 0; i < 10; ++i) {
        multiFrameAnimation.process();
        MockSprite currentFrame = multiFrameAnimation.getCurrentFrame();
        EXPECT_NE(currentFrame.getTexture(), nullptr);
    }
}

// Test utility functions integration
TEST_F(SimpleIntegrationTest, UtilityFunctionsIntegration) {
    // Test resource creation utility
    Resources testResources = IntegrationTestUtils::createResources(500, 250, 50);
    EXPECT_EQ(testResources.gold(), 500);
    EXPECT_EQ(testResources.lumber(), 250);
    EXPECT_EQ(testResources.oil(), 50);

    // Test test map data utility
    IntegrationTestUtils::TestMapData mapData(10, 10);
    EXPECT_EQ(mapData.width, 10);
    EXPECT_EQ(mapData.height, 10);

    // Set and get tile data
    mapData.setTile(5, 5, 100);
    EXPECT_EQ(mapData.getTile(5, 5), 100);

    // Out of bounds should return 0
    EXPECT_EQ(mapData.getTile(-1, 0), 0);
    EXPECT_EQ(mapData.getTile(15, 5), 0);
}

// Test data structure integration
TEST_F(SimpleIntegrationTest, DataStructureIntegration) {
    std::vector<Resources> resourceList;
    resourceList.emplace_back(100, 50, 0);
    resourceList.emplace_back(200, 100, 25);
    resourceList.emplace_back(300, 150, 50);

    EXPECT_EQ(resourceList.size(), 3);

    // Test resource accumulation
    Resources total(0, 0, 0);
    for (const auto& res : resourceList) {
        total += res;
    }

    EXPECT_EQ(total.gold(), 600);
    EXPECT_EQ(total.lumber(), 300);
    EXPECT_EQ(total.oil(), 75);
}

// Test simple game logic integration
TEST_F(SimpleIntegrationTest, SimpleGameLogicIntegration) {
    // Simulate a simple economic transaction
    Resources playerGold(1000, 0, 0);
    Resources buildingCost(300, 150, 0);
    Resources additionalLumber(0, 200, 0);

    // Player gets lumber from somewhere
    playerGold += additionalLumber;
    EXPECT_EQ(playerGold.lumber(), 200);

    // Player can afford building
    EXPECT_TRUE(playerGold >= buildingCost);

    // Purchase building
    playerGold -= buildingCost;
    EXPECT_EQ(playerGold.gold(), 700);
    EXPECT_EQ(playerGold.lumber(), 50);
}

// Test error handling and edge cases
TEST_F(SimpleIntegrationTest, ErrorHandlingIntegration) {
    Resources emptyResources(0, 0, 0);
    Resources negativeCost(-100, -50, -25);

    // Test with negative values (edge case)
    emptyResources += negativeCost;
    EXPECT_EQ(emptyResources.gold(), -100);
    EXPECT_EQ(emptyResources.lumber(), -50);
    EXPECT_EQ(emptyResources.oil(), -25);

    // Test comparison with negative resources
    Resources someResources(50, 25, 10);
    EXPECT_FALSE(emptyResources >= someResources);
    EXPECT_TRUE(someResources >= emptyResources);
}

// Test performance with multiple operations
TEST_F(SimpleIntegrationTest, PerformanceIntegration) {
    const int operationCount = 1000;
    std::vector<Resources> resourceVector;

    // Create many resource objects
    for (int i = 0; i < operationCount; ++i) {
        resourceVector.emplace_back(i, i * 2, i / 2);
    }

    EXPECT_EQ(resourceVector.size(), operationCount);

    // Perform operations on all resources
    Resources accumulator(0, 0, 0);
    for (const auto& res : resourceVector) {
        accumulator += res;
        if (res.gold() > 500) {
            accumulator -= Resources(10, 5, 1);
        }
    }

    // Should complete without issues
    EXPECT_GT(accumulator.gold(), 0);
}

// Test complex resource chain
TEST_F(SimpleIntegrationTest, ResourceChainIntegration) {
    // Simulate resource chain: mine -> process -> trade -> build

    // Mining
    Resources minedResources(100, 200, 50);

    // Processing (costs some resources, produces others)
    Resources processingCost(10, 0, 5);
    Resources processedOutput(0, 50, 100);

    if (minedResources >= processingCost) {
        minedResources -= processingCost;
        minedResources += processedOutput;
    }

    EXPECT_EQ(minedResources.gold(), 90);
    EXPECT_EQ(minedResources.lumber(), 250);
    EXPECT_EQ(minedResources.oil(), 145);

    // Trading
    Resources tradeOffer(50, 0, 0);
    Resources tradeReceive(0, 100, 0);

    if (minedResources >= tradeOffer) {
        minedResources -= tradeOffer;
        minedResources += tradeReceive;
    }

    EXPECT_EQ(minedResources.gold(), 40);
    EXPECT_EQ(minedResources.lumber(), 350);

    // Building
    Resources buildingCost(40, 300, 100);
    bool canBuild = minedResources >= buildingCost;

    EXPECT_TRUE(canBuild);

    if (canBuild) {
        minedResources -= buildingCost;
    }

    EXPECT_EQ(minedResources.gold(), 0);
    EXPECT_EQ(minedResources.lumber(), 50);
    EXPECT_EQ(minedResources.oil(), 45);
}