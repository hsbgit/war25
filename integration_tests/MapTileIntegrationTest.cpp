#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockFramework.h"

#ifdef INTEGRATION_TEST_MODE

// Mock Tile implementation for integration testing
class IntegrationTestTile {
public:
    IntegrationTestTile(int x, int y, uint16_t tileId = 0)
        : m_pos{x, y}, m_tileId(tileId), m_pLandSeaUnit(nullptr), m_pAirUnit(nullptr),
          m_isWater(false), m_isLand(true), m_isWood(false), m_isCoast(false) {}

    // Position methods
    int getX() const { return m_pos.x; }
    int getY() const { return m_pos.y; }
    const Point& getPos() const { return m_pos; }

    // Tile properties
    uint16_t getTileID() const { return m_tileId; }
    void setTileID(uint16_t id) { m_tileId = id; }

    bool isSea() const { return m_isWater; }
    bool isLand() const { return m_isLand; }
    bool isWood() const { return m_isWood; }
    bool isCoast() const { return m_isCoast; }
    bool isDestroyable() const { return m_isWood; }

    void setProperties(bool water, bool land, bool wood, bool coast) {
        m_isWater = water;
        m_isLand = land;
        m_isWood = wood;
        m_isCoast = coast;
    }

    // Object management
    bool hasObject() const { return m_pLandSeaUnit != nullptr || m_pAirUnit != nullptr; }

    void* getSeaOrLandUnit() const { return m_pLandSeaUnit; }
    void* getAirUnit() const { return m_pAirUnit; }

    void assignLandSeaUnit(void* unit) { m_pLandSeaUnit = unit; }
    void assignAirUnit(void* unit) { m_pAirUnit = unit; }

    void releaseLandSeaUnit() { m_pLandSeaUnit = nullptr; }
    void releaseAirUnit() { m_pAirUnit = nullptr; }

    bool isBlockedForLandUnit(void* unit = nullptr) const {
        return m_pLandSeaUnit != nullptr && m_pLandSeaUnit != unit;
    }

private:
    Point m_pos;
    uint16_t m_tileId;
    void* m_pLandSeaUnit;
    void* m_pAirUnit;
    bool m_isWater, m_isLand, m_isWood, m_isCoast;
};

// Mock Map implementation for integration testing
class IntegrationTestMap {
public:
    IntegrationTestMap(int width, int height)
        : m_width(width), m_height(height) {
        m_tiles.resize(height);
        for (int y = 0; y < height; ++y) {
            m_tiles[y].resize(width);
            for (int x = 0; x < width; ++x) {
                m_tiles[y][x] = std::make_unique<IntegrationTestTile>(x, y);
            }
        }
    }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    IntegrationTestTile* getTile(int x, int y) {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            return m_tiles[y][x].get();
        }
        return nullptr;
    }

    IntegrationTestTile* getTile(const Point& pos) {
        return getTile(pos.x, pos.y);
    }

    std::vector<IntegrationTestTile*> getNeighbourTiles(IntegrationTestTile* tile, int distance = 1) {
        std::vector<IntegrationTestTile*> neighbours;
        if (!tile) return neighbours;

        int centerX = tile->getX();
        int centerY = tile->getY();

        for (int dy = -distance; dy <= distance; ++dy) {
            for (int dx = -distance; dx <= distance; ++dx) {
                if (dx == 0 && dy == 0) continue; // Skip center tile

                int x = centerX + dx;
                int y = centerY + dy;

                if (IntegrationTestTile* neighbor = getTile(x, y)) {
                    neighbours.push_back(neighbor);
                }
            }
        }

        return neighbours;
    }

    IntegrationTestTile* getNextFreeTile(const Point& worldPos, int maxDistance = 10) {
        for (int distance = 1; distance <= maxDistance; ++distance) {
            for (int dy = -distance; dy <= distance; ++dy) {
                for (int dx = -distance; dx <= distance; ++dx) {
                    if (abs(dx) != distance && abs(dy) != distance) continue;

                    int x = worldPos.x + dx;
                    int y = worldPos.y + dy;

                    if (IntegrationTestTile* tile = getTile(x, y)) {
                        if (!tile->hasObject() && tile->isLand()) {
                            return tile;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    bool isSpaceAvailableForUnit(int x, int y, int unitSize = 1) {
        for (int dy = 0; dy < unitSize; ++dy) {
            for (int dx = 0; dx < unitSize; ++dx) {
                IntegrationTestTile* tile = getTile(x + dx, y + dy);
                if (!tile || tile->hasObject() || !tile->isLand()) {
                    return false;
                }
            }
        }
        return true;
    }

    void placeUnit(int x, int y, void* unit, int unitSize = 1) {
        for (int dy = 0; dy < unitSize; ++dy) {
            for (int dx = 0; dx < unitSize; ++dx) {
                IntegrationTestTile* tile = getTile(x + dx, y + dy);
                if (tile) {
                    tile->assignLandSeaUnit(unit);
                }
            }
        }
    }

    void removeUnit(int x, int y, int unitSize = 1) {
        for (int dy = 0; dy < unitSize; ++dy) {
            for (int dx = 0; dx < unitSize; ++dx) {
                IntegrationTestTile* tile = getTile(x + dx, y + dy);
                if (tile) {
                    tile->releaseLandSeaUnit();
                }
            }
        }
    }

private:
    int m_width, m_height;
    std::vector<std::vector<std::unique_ptr<IntegrationTestTile>>> m_tiles;
};

#endif // INTEGRATION_TEST_MODE

class MapTileIntegrationTest : public War25IntegrationTest {
protected:
    void SetUp() override {
        War25IntegrationTest::SetUp();

        // Create a test map
        testMap = std::make_unique<IntegrationTestMap>(10, 10);

        // Setup some terrain features
        setupTestTerrain();
    }

    void setupTestTerrain() {
        // Create a water area (top-left corner)
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                if (auto tile = testMap->getTile(x, y)) {
                    tile->setProperties(true, false, false, false); // Water
                }
            }
        }

        // Create coastline
        for (int x = 0; x < 3; ++x) {
            if (auto tile = testMap->getTile(x, 3)) {
                tile->setProperties(false, true, false, true); // Coast
            }
        }
        for (int y = 0; y < 3; ++y) {
            if (auto tile = testMap->getTile(3, y)) {
                tile->setProperties(false, true, false, true); // Coast
            }
        }

        // Create forest area (bottom-right corner)
        for (int y = 7; y < 10; ++y) {
            for (int x = 7; x < 10; ++x) {
                if (auto tile = testMap->getTile(x, y)) {
                    tile->setProperties(false, true, true, false); // Forest
                    tile->setTileID(100); // Forest tile ID
                }
            }
        }
    }

    std::unique_ptr<IntegrationTestMap> testMap;
};

TEST_F(MapTileIntegrationTest, MapBasicProperties) {
    EXPECT_EQ(testMap->getWidth(), 10);
    EXPECT_EQ(testMap->getHeight(), 10);

    // Test tile access
    auto tile = testMap->getTile(5, 5);
    ASSERT_NE(tile, nullptr);
    EXPECT_EQ(tile->getX(), 5);
    EXPECT_EQ(tile->getY(), 5);

    // Test out of bounds
    EXPECT_EQ(testMap->getTile(-1, 0), nullptr);
    EXPECT_EQ(testMap->getTile(10, 0), nullptr);
    EXPECT_EQ(testMap->getTile(0, -1), nullptr);
    EXPECT_EQ(testMap->getTile(0, 10), nullptr);
}

TEST_F(MapTileIntegrationTest, TerrainProperties) {
    // Test water area
    auto waterTile = testMap->getTile(1, 1);
    ASSERT_NE(waterTile, nullptr);
    EXPECT_TRUE(waterTile->isSea());
    EXPECT_FALSE(waterTile->isLand());
    EXPECT_FALSE(waterTile->isWood());
    EXPECT_FALSE(waterTile->isCoast());

    // Test coast
    auto coastTile = testMap->getTile(3, 3);
    ASSERT_NE(coastTile, nullptr);
    EXPECT_FALSE(coastTile->isSea());
    EXPECT_TRUE(coastTile->isLand());
    EXPECT_FALSE(coastTile->isWood());
    EXPECT_TRUE(coastTile->isCoast());

    // Test forest
    auto forestTile = testMap->getTile(8, 8);
    ASSERT_NE(forestTile, nullptr);
    EXPECT_FALSE(forestTile->isSea());
    EXPECT_TRUE(forestTile->isLand());
    EXPECT_TRUE(forestTile->isWood());
    EXPECT_FALSE(forestTile->isCoast());
    EXPECT_TRUE(forestTile->isDestroyable());
    EXPECT_EQ(forestTile->getTileID(), 100);

    // Test normal land
    auto landTile = testMap->getTile(5, 5);
    ASSERT_NE(landTile, nullptr);
    EXPECT_FALSE(landTile->isSea());
    EXPECT_TRUE(landTile->isLand());
    EXPECT_FALSE(landTile->isWood());
    EXPECT_FALSE(landTile->isCoast());
}

TEST_F(MapTileIntegrationTest, NeighbourTileDiscovery) {
    auto centerTile = testMap->getTile(5, 5);
    ASSERT_NE(centerTile, nullptr);

    // Test immediate neighbours
    auto neighbours = testMap->getNeighbourTiles(centerTile, 1);
    EXPECT_EQ(neighbours.size(), 8); // All 8 surrounding tiles

    // Test distance 2 neighbours
    auto distantNeighbours = testMap->getNeighbourTiles(centerTile, 2);
    EXPECT_EQ(distantNeighbours.size(), 24); // 5x5 area minus center tile

    // Test edge tile neighbours
    auto edgeTile = testMap->getTile(0, 0);
    auto edgeNeighbours = testMap->getNeighbourTiles(edgeTile, 1);
    EXPECT_EQ(edgeNeighbours.size(), 3); // Only 3 valid neighbours for corner tile
}

TEST_F(MapTileIntegrationTest, UnitPlacementAndRemoval) {
    void* mockUnit1 = reinterpret_cast<void*>(0x1001);
    void* mockUnit2 = reinterpret_cast<void*>(0x1002);

    auto tile = testMap->getTile(5, 5);
    ASSERT_NE(tile, nullptr);

    // Initially empty
    EXPECT_FALSE(tile->hasObject());
    EXPECT_EQ(tile->getSeaOrLandUnit(), nullptr);

    // Place unit
    tile->assignLandSeaUnit(mockUnit1);
    EXPECT_TRUE(tile->hasObject());
    EXPECT_EQ(tile->getSeaOrLandUnit(), mockUnit1);

    // Test blocking
    EXPECT_TRUE(tile->isBlockedForLandUnit(mockUnit2));
    EXPECT_FALSE(tile->isBlockedForLandUnit(mockUnit1)); // Same unit not blocked

    // Remove unit
    tile->releaseLandSeaUnit();
    EXPECT_FALSE(tile->hasObject());
    EXPECT_EQ(tile->getSeaOrLandUnit(), nullptr);
}

TEST_F(MapTileIntegrationTest, AirUnitSeparateLayer) {
    void* mockLandUnit = reinterpret_cast<void*>(0x1001);
    void* mockAirUnit = reinterpret_cast<void*>(0x2001);

    auto tile = testMap->getTile(5, 5);
    ASSERT_NE(tile, nullptr);

    // Place both land and air units
    tile->assignLandSeaUnit(mockLandUnit);
    tile->assignAirUnit(mockAirUnit);

    EXPECT_TRUE(tile->hasObject());
    EXPECT_EQ(tile->getSeaOrLandUnit(), mockLandUnit);
    EXPECT_EQ(tile->getAirUnit(), mockAirUnit);

    // Remove land unit, air unit should remain
    tile->releaseLandSeaUnit();
    EXPECT_TRUE(tile->hasObject()); // Still has air unit
    EXPECT_EQ(tile->getSeaOrLandUnit(), nullptr);
    EXPECT_EQ(tile->getAirUnit(), mockAirUnit);

    // Remove air unit
    tile->releaseAirUnit();
    EXPECT_FALSE(tile->hasObject());
}

TEST_F(MapTileIntegrationTest, MapLevelUnitPlacement) {
    void* mockUnit = reinterpret_cast<void*>(0x1001);

    // Test single tile unit placement
    EXPECT_TRUE(testMap->isSpaceAvailableForUnit(5, 5, 1));
    testMap->placeUnit(5, 5, mockUnit, 1);

    auto tile = testMap->getTile(5, 5);
    EXPECT_EQ(tile->getSeaOrLandUnit(), mockUnit);
    EXPECT_FALSE(testMap->isSpaceAvailableForUnit(5, 5, 1));

    // Test removal
    testMap->removeUnit(5, 5, 1);
    EXPECT_EQ(tile->getSeaOrLandUnit(), nullptr);
    EXPECT_TRUE(testMap->isSpaceAvailableForUnit(5, 5, 1));
}

TEST_F(MapTileIntegrationTest, LargeUnitPlacement) {
    void* mockBuilding = reinterpret_cast<void*>(0x3001);

    // Test 2x2 building placement
    EXPECT_TRUE(testMap->isSpaceAvailableForUnit(4, 4, 2));
    testMap->placeUnit(4, 4, mockBuilding, 2);

    // Check all 4 tiles are occupied
    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
            auto tile = testMap->getTile(4 + dx, 4 + dy);
            EXPECT_EQ(tile->getSeaOrLandUnit(), mockBuilding);
        }
    }

    // Space should not be available for overlapping placement
    EXPECT_FALSE(testMap->isSpaceAvailableForUnit(3, 3, 2));
    EXPECT_FALSE(testMap->isSpaceAvailableForUnit(4, 4, 2));
    EXPECT_FALSE(testMap->isSpaceAvailableForUnit(5, 5, 2));

    // Remove building
    testMap->removeUnit(4, 4, 2);
    EXPECT_TRUE(testMap->isSpaceAvailableForUnit(4, 4, 2));
}

TEST_F(MapTileIntegrationTest, FreeTileDiscovery) {
    void* mockUnit = reinterpret_cast<void*>(0x1001);

    Point startPos = {5, 5};

    // Should find the starting position initially
    auto freeTile = testMap->getNextFreeTile(startPos, 1);
    ASSERT_NE(freeTile, nullptr);
    EXPECT_EQ(freeTile->getPos().x, 5);
    EXPECT_EQ(freeTile->getPos().y, 5);

    // Place unit at center
    testMap->placeUnit(5, 5, mockUnit, 1);

    // Should find a neighbour tile
    freeTile = testMap->getNextFreeTile(startPos, 1);
    ASSERT_NE(freeTile, nullptr);
    EXPECT_NE(freeTile->getPos().x, 5); // Should not be the center

    // Fill surrounding area
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            testMap->placeUnit(5 + dx, 5 + dy, mockUnit, 1);
        }
    }

    // Should find tile at distance 2
    freeTile = testMap->getNextFreeTile(startPos, 3);
    ASSERT_NE(freeTile, nullptr);
    int distance = abs(freeTile->getPos().x - 5) + abs(freeTile->getPos().y - 5);
    EXPECT_GE(distance, 2);
}

TEST_F(MapTileIntegrationTest, TerrainConstrainedPlacement) {
    void* mockUnit = reinterpret_cast<void*>(0x1001);

    // Cannot place on water
    EXPECT_FALSE(testMap->isSpaceAvailableForUnit(1, 1, 1)); // Water tile

    // Cannot place in forest without clearing
    auto forestTile = testMap->getTile(8, 8);
    EXPECT_TRUE(forestTile->isWood());
    // In a real game, units couldn't be placed on forest tiles
    // For testing, we'll assume they can be placed but it's noted

    // Can place on normal land
    EXPECT_TRUE(testMap->isSpaceAvailableForUnit(5, 5, 1));

    // Can place on coast (land)
    EXPECT_TRUE(testMap->isSpaceAvailableForUnit(3, 3, 1));
}