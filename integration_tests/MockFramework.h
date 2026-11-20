#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include <map>
#include <cstdint>

// Mock SDL types for integration testing
#ifdef INTEGRATION_TEST_MODE

struct SDL_Point {
    int x, y;

    bool operator==(const SDL_Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const SDL_Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct SDL_Rect {
    int x, y, w, h;
};

typedef SDL_Point Point;
typedef SDL_Rect Rect;

// Mock Texture class
class MockTexture {
public:
    MockTexture(int width = 32, int height = 32) : m_width(width), m_height(height) {}
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width, m_height;
};

// Mock Sprite class
class MockSprite {
public:
    MockSprite() : m_texture(std::make_shared<MockTexture>()) {}
    MockSprite(std::shared_ptr<MockTexture> texture) : m_texture(texture) {}

    MockTexture* getTexture() const { return m_texture.get(); }
    void setTexture(std::shared_ptr<MockTexture> texture) { m_texture = texture; }

private:
    std::shared_ptr<MockTexture> m_texture;
};

// Mock ResourceManager
class MockResourceManager {
public:
    virtual ~MockResourceManager() = default;

    std::shared_ptr<MockTexture> getTexture(const std::string& name) {
        auto it = m_textures.find(name);
        if (it != m_textures.end()) {
            return it->second;
        }
        auto texture = std::make_shared<MockTexture>();
        m_textures[name] = texture;
        return texture;
    }

private:
    std::map<std::string, std::shared_ptr<MockTexture>> m_textures;
};

// Mock Animation class
class MockAnimation {
public:
    MockAnimation() : m_currentFrame(0), m_frameCount(1) {}
    MockAnimation(int frameCount) : m_currentFrame(0), m_frameCount(frameCount) {}

    MockSprite getCurrentFrame() const { return MockSprite(); }
    void process() { m_currentFrame = (m_currentFrame + 1) % m_frameCount; }

private:
    int m_currentFrame;
    int m_frameCount;
};

#endif // INTEGRATION_TEST_MODE

// Base Integration Test class
class War25IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockResourceManager = std::make_unique<MockResourceManager>();
    }

    void TearDown() override {
        mockResourceManager.reset();
    }

    // Helper methods for creating test objects
    Point createPoint(int x, int y) {
        return Point{x, y};
    }

    std::unique_ptr<MockResourceManager> mockResourceManager;
};

// Forward declaration of Resources class
class Resources {
public:
    Resources(int gold = 0, int lumber = 0, int oil = 0) : m_gold(gold), m_lumber(lumber), m_oil(oil) {}

    bool operator>=(const Resources& other) const {
        return m_gold >= other.m_gold && m_lumber >= other.m_lumber && m_oil >= other.m_oil;
    }

    Resources& operator+=(const Resources& other) {
        m_gold += other.m_gold;
        m_lumber += other.m_lumber;
        m_oil += other.m_oil;
        return *this;
    }

    Resources& operator-=(const Resources& other) {
        m_gold -= other.m_gold;
        m_lumber -= other.m_lumber;
        m_oil -= other.m_oil;
        return *this;
    }

    int gold() const { return m_gold; }
    int lumber() const { return m_lumber; }
    int oil() const { return m_oil; }

private:
    int m_gold, m_lumber, m_oil;
};

// Player colors and fractions
enum class PlayerColor {Red=0, Blue, Green, Violet, Orange, Black, White, Yellow, Neutral=15};
enum class Fraction {Neutral, Alliance, Orc};

// Integration Test Utilities
namespace IntegrationTestUtils {

    // Helper to create test resources
    inline Resources createResources(int gold, int lumber = 0, int oil = 0) {
        return Resources(gold, lumber, oil);
    }

    // Helper to create test points
    inline Point createTestPoint(int x, int y) {
        return Point{x, y};
    }

    // Helper to compare points
    inline bool pointsEqual(const Point& p1, const Point& p2) {
        return p1.x == p2.x && p1.y == p2.y;
    }

    // Helper to create simple test map data
    struct TestMapData {
        int width, height;
        std::vector<std::vector<uint16_t>> tileIds;

        TestMapData(int w, int h) : width(w), height(h) {
            tileIds.resize(h);
            for (int y = 0; y < h; ++y) {
                tileIds[y].resize(w, 0);
            }
        }

        void setTile(int x, int y, uint16_t tileId) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                tileIds[y][x] = tileId;
            }
        }

        uint16_t getTile(int x, int y) const {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                return tileIds[y][x];
            }
            return 0;
        }
    };
}