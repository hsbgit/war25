#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockFramework.h"
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#ifdef INTEGRATION_TEST_MODE

// Direction enum for pathfinding
enum class TestDirection { N = 0, NE, E, SE, S, SW, W, NW };

// Node interface for pathfinding
class IntegrationTestNode {
public:
    IntegrationTestNode(int x, int y) : m_x(x), m_y(y), m_g(0), m_h(0), m_parent(nullptr) {}

    int getX() const { return m_x; }
    int getY() const { return m_y; }

    // A* algorithm values
    int getG() const { return m_g; }
    int getH() const { return m_h; }
    int getF() const { return m_g + m_h; }

    void setG(int g) { m_g = g; }
    void setH(int h) { m_h = h; }
    void setParent(IntegrationTestNode* parent) { m_parent = parent; }

    IntegrationTestNode* getParent() const { return m_parent; }

    // Get neighboring nodes for pathfinding
    std::vector<IntegrationTestNode*> getNeighbors(const std::vector<std::vector<IntegrationTestNode*>>& grid) const {
        std::vector<IntegrationTestNode*> neighbors;
        int gridHeight = static_cast<int>(grid.size());
        int gridWidth = gridHeight > 0 ? static_cast<int>(grid[0].size()) : 0;

        // 8-directional movement
        int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
        int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

        for (int i = 0; i < 8; ++i) {
            int newX = m_x + dx[i];
            int newY = m_y + dy[i];

            if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight) {
                neighbors.push_back(grid[newY][newX]);
            }
        }

        return neighbors;
    }

    // Calculate Manhattan distance heuristic
    int calculateManhattanDistance(const IntegrationTestNode* goal) const {
        return abs(m_x - goal->getX()) + abs(m_y - goal->getY());
    }

    // Calculate Euclidean distance heuristic
    int calculateEuclideanDistance(const IntegrationTestNode* goal) const {
        int dx = m_x - goal->getX();
        int dy = m_y - goal->getY();
        return static_cast<int>(sqrt(dx * dx + dy * dy) * 10); // Multiply by 10 for integer math
    }

private:
    int m_x, m_y;
    int m_g, m_h;
    IntegrationTestNode* m_parent;
};

// Pathfinding map with obstacles
class PathfindingMap {
public:
    PathfindingMap(int width, int height) : m_width(width), m_height(height) {
        m_nodes.resize(height);
        m_obstacles.resize(height);

        for (int y = 0; y < height; ++y) {
            m_nodes[y].resize(width);
            m_obstacles[y].resize(width, false);

            for (int x = 0; x < width; ++x) {
                m_nodes[y][x] = new IntegrationTestNode(x, y);
            }
        }
    }

    ~PathfindingMap() {
        for (auto& row : m_nodes) {
            for (auto* node : row) {
                delete node;
            }
        }
    }

    IntegrationTestNode* getNode(int x, int y) {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            return m_nodes[y][x];
        }
        return nullptr;
    }

    void setObstacle(int x, int y, bool isObstacle) {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            m_obstacles[y][x] = isObstacle;
        }
    }

    bool isObstacle(int x, int y) const {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            return m_obstacles[y][x];
        }
        return true; // Out of bounds is considered obstacle
    }

    bool isWalkable(IntegrationTestNode* node) const {
        return node && !isObstacle(node->getX(), node->getY());
    }

    std::vector<IntegrationTestNode*> getWalkableNeighbors(IntegrationTestNode* node) const {
        auto neighbors = node->getNeighbors(m_nodes);
        neighbors.erase(
            std::remove_if(neighbors.begin(), neighbors.end(),
                [this](IntegrationTestNode* n) { return !isWalkable(n); }),
            neighbors.end()
        );
        return neighbors;
    }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width, m_height;
    std::vector<std::vector<IntegrationTestNode*>> m_nodes;
    std::vector<std::vector<bool>> m_obstacles;
};

// A* Pathfinding implementation
class IntegrationAStar {
public:
    std::vector<IntegrationTestNode*> findPath(PathfindingMap* map, IntegrationTestNode* start, IntegrationTestNode* goal) {
        if (!map || !start || !goal || !map->isWalkable(start) || !map->isWalkable(goal)) {
            return {};
        }

        // Reset all nodes
        for (int y = 0; y < map->getHeight(); ++y) {
            for (int x = 0; x < map->getWidth(); ++x) {
                auto node = map->getNode(x, y);
                node->setG(0);
                node->setH(0);
                node->setParent(nullptr);
            }
        }

        std::priority_queue<IntegrationTestNode*, std::vector<IntegrationTestNode*>, NodeComparator> openSet;
        std::unordered_set<IntegrationTestNode*> openSetLookup;
        std::unordered_set<IntegrationTestNode*> closedSet;

        start->setG(0);
        start->setH(start->calculateManhattanDistance(goal));
        openSet.push(start);
        openSetLookup.insert(start);

        while (!openSet.empty()) {
            IntegrationTestNode* current = openSet.top();
            openSet.pop();
            openSetLookup.erase(current);

            if (current == goal) {
                return reconstructPath(goal);
            }

            closedSet.insert(current);

            auto neighbors = map->getWalkableNeighbors(current);
            for (auto neighbor : neighbors) {
                if (closedSet.find(neighbor) != closedSet.end()) {
                    continue;
                }

                int tentativeG = current->getG() + getMovementCost(current, neighbor);

                if (openSetLookup.find(neighbor) == openSetLookup.end()) {
                    neighbor->setG(tentativeG);
                    neighbor->setH(neighbor->calculateManhattanDistance(goal));
                    neighbor->setParent(current);
                    openSet.push(neighbor);
                    openSetLookup.insert(neighbor);
                } else if (tentativeG < neighbor->getG()) {
                    neighbor->setG(tentativeG);
                    neighbor->setParent(current);
                    // Note: Priority queue doesn't support decrease key, so this is simplified
                }
            }
        }

        return {}; // No path found
    }

private:
    struct NodeComparator {
        bool operator()(IntegrationTestNode* a, IntegrationTestNode* b) {
            return a->getF() > b->getF(); // Min-heap based on F score
        }
    };

    int getMovementCost(IntegrationTestNode* from, IntegrationTestNode* to) {
        int dx = abs(to->getX() - from->getX());
        int dy = abs(to->getY() - from->getY());

        if (dx == 1 && dy == 1) {
            return 14; // Diagonal movement cost (sqrt(2) * 10 ≈ 14)
        }
        return 10; // Straight movement cost
    }

    std::vector<IntegrationTestNode*> reconstructPath(IntegrationTestNode* goal) {
        std::vector<IntegrationTestNode*> path;
        IntegrationTestNode* current = goal;

        while (current) {
            path.push_back(current);
            current = current->getParent();
        }

        std::reverse(path.begin(), path.end());
        return path;
    }
};

#endif // INTEGRATION_TEST_MODE

class PathfindingIntegrationTest : public War25IntegrationTest {
protected:
    void SetUp() override {
        War25IntegrationTest::SetUp();

        // Create test map
        pathfindingMap = std::make_unique<PathfindingMap>(10, 10);
        pathfinder = std::make_unique<IntegrationAStar>();

        // Setup test scenarios
        setupBasicMap();
    }

    void setupBasicMap() {
        // Create a wall obstacle
        for (int y = 2; y < 8; ++y) {
            pathfindingMap->setObstacle(5, y, true);
        }

        // Create some scattered obstacles
        pathfindingMap->setObstacle(2, 2, true);
        pathfindingMap->setObstacle(3, 2, true);
        pathfindingMap->setObstacle(7, 7, true);
        pathfindingMap->setObstacle(8, 6, true);
    }

    std::unique_ptr<PathfindingMap> pathfindingMap;
    std::unique_ptr<IntegrationAStar> pathfinder;
};

TEST_F(PathfindingIntegrationTest, BasicPathfinding) {
    auto start = pathfindingMap->getNode(0, 0);
    auto goal = pathfindingMap->getNode(9, 9);

    auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.front(), start);
    EXPECT_EQ(path.back(), goal);

    // Verify path continuity
    for (size_t i = 1; i < path.size(); ++i) {
        int dx = abs(path[i]->getX() - path[i-1]->getX());
        int dy = abs(path[i]->getY() - path[i-1]->getY());
        EXPECT_LE(dx, 1);
        EXPECT_LE(dy, 1);
        EXPECT_TRUE(dx + dy > 0); // Should move at least one step
    }
}

TEST_F(PathfindingIntegrationTest, StraightLinePathfinding) {
    auto start = pathfindingMap->getNode(0, 0);
    auto goal = pathfindingMap->getNode(3, 0);

    auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.size(), 4); // Should be straight line: (0,0), (1,0), (2,0), (3,0)

    for (size_t i = 0; i < path.size(); ++i) {
        EXPECT_EQ(path[i]->getX(), static_cast<int>(i));
        EXPECT_EQ(path[i]->getY(), 0);
    }
}

TEST_F(PathfindingIntegrationTest, ObstacleAvoidance) {
    auto start = pathfindingMap->getNode(4, 5);
    auto goal = pathfindingMap->getNode(6, 5);

    auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.front(), start);
    EXPECT_EQ(path.back(), goal);

    // Path should go around the wall at x=5
    bool hasRouteAroundObstacle = false;
    for (auto node : path) {
        if (node->getX() != 5) {
            hasRouteAroundObstacle = true;
        }
        // Verify path doesn't go through obstacles
        EXPECT_FALSE(pathfindingMap->isObstacle(node->getX(), node->getY()));
    }
    EXPECT_TRUE(hasRouteAroundObstacle);
}

TEST_F(PathfindingIntegrationTest, NoPathAvailable) {
    // Create a completely enclosed area
    for (int x = 0; x < 10; ++x) {
        pathfindingMap->setObstacle(x, 4, true);
    }

    auto start = pathfindingMap->getNode(5, 0);
    auto goal = pathfindingMap->getNode(5, 9);

    auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

    EXPECT_TRUE(path.empty()); // No path should be found
}

TEST_F(PathfindingIntegrationTest, SameStartAndGoal) {
    auto start = pathfindingMap->getNode(5, 5);
    auto goal = pathfindingMap->getNode(5, 5);

    auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.size(), 1);
    EXPECT_EQ(path[0], start);
}

TEST_F(PathfindingIntegrationTest, InvalidStartOrGoal) {
    auto start = pathfindingMap->getNode(5, 2); // On obstacle
    auto goal = pathfindingMap->getNode(9, 9);

    auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

    EXPECT_TRUE(path.empty()); // Should fail with invalid start
}

TEST_F(PathfindingIntegrationTest, OptimalPathLength) {
    // Clear map for optimal path testing
    auto clearMap = std::make_unique<PathfindingMap>(5, 5);

    auto start = clearMap->getNode(0, 0);
    auto goal = clearMap->getNode(4, 4);

    auto path = pathfinder->findPath(clearMap.get(), start, goal);

    ASSERT_FALSE(path.empty());

    // In a clear 5x5 map from (0,0) to (4,4), optimal path should be diagonal
    EXPECT_EQ(path.size(), 5); // Should be: (0,0), (1,1), (2,2), (3,3), (4,4)

    for (size_t i = 0; i < path.size(); ++i) {
        EXPECT_EQ(path[i]->getX(), static_cast<int>(i));
        EXPECT_EQ(path[i]->getY(), static_cast<int>(i));
    }
}

TEST_F(PathfindingIntegrationTest, LShapeObstacleNavigation) {
    // Create L-shaped obstacle
    auto lShapeMap = std::make_unique<PathfindingMap>(7, 7);

    // Horizontal part of L
    for (int x = 2; x <= 5; ++x) {
        lShapeMap->setObstacle(x, 3, true);
    }
    // Vertical part of L
    for (int y = 3; y <= 5; ++y) {
        lShapeMap->setObstacle(5, y, true);
    }

    auto start = lShapeMap->getNode(1, 3);
    auto goal = lShapeMap->getNode(6, 3);

    auto path = pathfinder->findPath(lShapeMap.get(), start, goal);

    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.front(), start);
    EXPECT_EQ(path.back(), goal);

    // Path should navigate around the L-obstacle
    bool routesAroundTop = false, routesAroundBottom = false;
    for (auto node : path) {
        if (node->getY() < 3) routesAroundTop = true;
        if (node->getY() > 3) routesAroundBottom = true;
        EXPECT_FALSE(lShapeMap->isObstacle(node->getX(), node->getY()));
    }

    // Should route around either top or bottom
    EXPECT_TRUE(routesAroundTop || routesAroundBottom);
}

TEST_F(PathfindingIntegrationTest, MazeNavigation) {
    // Create a simple maze
    auto mazeMap = std::make_unique<PathfindingMap>(9, 9);

    // Create maze walls
    for (int x = 1; x < 8; x += 2) {
        for (int y = 1; y < 8; ++y) {
            if (y != 4) { // Leave opening in middle
                mazeMap->setObstacle(x, y, true);
            }
        }
    }

    auto start = mazeMap->getNode(0, 4);
    auto goal = mazeMap->getNode(8, 4);

    auto path = pathfinder->findPath(mazeMap.get(), start, goal);

    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.front(), start);
    EXPECT_EQ(path.back(), goal);

    // Verify path doesn't go through walls
    for (auto node : path) {
        EXPECT_FALSE(mazeMap->isObstacle(node->getX(), node->getY()));
    }

    // Path should be reasonably efficient (not too much longer than optimal)
    int expectedMinLength = 9; // Straight line distance
    EXPECT_LE(static_cast<int>(path.size()), expectedMinLength + 5); // Allow some detour
}

TEST_F(PathfindingIntegrationTest, MultiplePathfindingRequests) {
    // Test multiple pathfinding requests to ensure state is properly reset
    std::vector<std::pair<Point, Point>> requests = {
        {{0, 0}, {9, 9}},
        {{9, 0}, {0, 9}},
        {{4, 0}, {4, 9}},
        {{0, 4}, {9, 4}}
    };

    for (const auto& request : requests) {
        auto start = pathfindingMap->getNode(request.first.x, request.first.y);
        auto goal = pathfindingMap->getNode(request.second.x, request.second.y);

        auto path = pathfinder->findPath(pathfindingMap.get(), start, goal);

        ASSERT_FALSE(path.empty()) << "Failed for path from ("
                                   << request.first.x << "," << request.first.y
                                   << ") to (" << request.second.x << "," << request.second.y << ")";
        EXPECT_EQ(path.front(), start);
        EXPECT_EQ(path.back(), goal);

        // Verify each path doesn't go through obstacles
        for (auto node : path) {
            EXPECT_FALSE(pathfindingMap->isObstacle(node->getX(), node->getY()));
        }
    }
}