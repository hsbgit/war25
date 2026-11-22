#include <gtest/gtest.h>

#include "Map/MovementRules.h"
#include "Map/PathSearch.h"

#include <array>
#include <algorithm>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class GridNode final {
public:
	GridNode(int x, int y) : x(x), y(y) {}
	int getX() const { return x; }
	int getY() const { return y; }
	int x;
	int y;
	std::unordered_map<int, GridNode*> neighbours;
};

class Grid {
public:
	Grid(int width, int height, bool diagonals = false) : width(width), height(height) {
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
				nodes.push_back(std::make_unique<GridNode>(x, y));
		const std::array<std::tuple<int, int, int>, 8> directions{{
			{0, -1, 0}, {1, -1, 1}, {1, 0, 2}, {1, 1, 3},
			{0, 1, 4}, {-1, 1, 5}, {-1, 0, 6}, {-1, -1, 7}
		}};
		for (auto& node : nodes) {
			for (const auto& [dx, dy, direction] : directions) {
				if (!diagonals && dx != 0 && dy != 0) continue;
				GridNode* other = at(node->x + dx, node->y + dy);
				if (other) node->neighbours[direction] = other;
			}
		}
	}

	GridNode* at(int x, int y) const {
		if (x < 0 || y < 0 || x >= width || y >= height) return nullptr;
		return nodes[y * width + x].get();
	}

	PathSearch::Result<GridNode> find(GridNode* start, GridNode* end,
		const std::unordered_set<GridNode*>& blocked = {},
		const std::unordered_set<GridNode*>& congested = {}) const {
		return PathSearch::find(start, end,
			[](GridNode* node) { return node->neighbours; },
			[&blocked](GridNode*, GridNode* to) { return !blocked.contains(to); },
			[&congested](int direction, GridNode* to) {
				const bool diagonal = direction == 1 || direction == 3 ||
					direction == 5 || direction == 7;
				return (diagonal ? 14 : 10) + (congested.contains(to) ? 200 : 0);
			},
			[end](GridNode* node) {
				struct Position { int x; int y; };
				return MovementRules::octileDistance(Position{node->getX(), node->getY()},
					Position{end->getX(), end->getY()});
			});
	}

	int width;
	int height;
	std::vector<std::unique_ptr<GridNode>> nodes;
};

} // namespace

TEST(PathSearchTest, FindsShortestStraightRoute) {
	Grid grid(5, 3);
	auto result = grid.find(grid.at(0, 1), grid.at(4, 1));
	ASSERT_EQ(result.path.size(), 4);
	EXPECT_EQ(result.path.back(), grid.at(4, 1));
}

TEST(PathSearchTest, RoutesAroundPermanentBuildingFootprint) {
	Grid grid(7, 5);
	std::unordered_set<GridNode*> building{grid.at(3, 1), grid.at(3, 2), grid.at(3, 3)};
	auto result = grid.find(grid.at(0, 2), grid.at(6, 2), building);
	ASSERT_FALSE(result.path.empty());
	for (GridNode* node : result.path) EXPECT_FALSE(building.contains(node));
	EXPECT_GT(result.path.size(), 6);
}

TEST(PathSearchTest, PrefersDetourAroundIdleUnit) {
	Grid grid(7, 3);
	std::unordered_set<GridNode*> idleUnit{grid.at(3, 1)};
	auto result = grid.find(grid.at(0, 1), grid.at(6, 1), {}, idleUnit);
	ASSERT_FALSE(result.path.empty());
	EXPECT_EQ(std::find(result.path.begin(), result.path.end(), grid.at(3, 1)), result.path.end());
}

TEST(PathSearchTest, KeepsRouteThroughTemporaryUnitInOneTileChokepoint) {
	Grid grid(5, 1);
	std::unordered_set<GridNode*> temporaryUnit{grid.at(2, 0)};
	auto result = grid.find(grid.at(0, 0), grid.at(4, 0), {}, temporaryUnit);
	ASSERT_EQ(result.path.size(), 4);
	EXPECT_NE(std::find(result.path.begin(), result.path.end(), grid.at(2, 0)), result.path.end());
}

TEST(PathSearchTest, ReturnsNoRouteWhenPermanentWallSealsMap) {
	Grid grid(5, 3);
	std::unordered_set<GridNode*> wall{grid.at(2, 0), grid.at(2, 1), grid.at(2, 2)};
	EXPECT_TRUE(grid.find(grid.at(0, 1), grid.at(4, 1), wall).path.empty());
}

TEST(PathSearchTest, CanReturnClosestReachableTileForBlockedDestination) {
	Grid grid(5, 3);
	std::unordered_set<GridNode*> wall{grid.at(3, 0), grid.at(3, 1), grid.at(3, 2)};
	auto result = PathSearch::find(grid.at(0, 1), grid.at(4, 1),
		[](GridNode* node) { return node->neighbours; },
		[&wall](GridNode*, GridNode* to) { return !wall.contains(to); },
		[](int, GridNode*) { return 10; },
		[end = grid.at(4, 1)](GridNode* node) {
			struct Position { int x; int y; };
			return MovementRules::octileDistance(
				Position{node->getX(), node->getY()},
				Position{end->getX(), end->getY()});
		}, true);
	ASSERT_FALSE(result.path.empty());
	EXPECT_FALSE(result.reachedGoal);
	EXPECT_EQ(result.endpoint, grid.at(2, 1));
	EXPECT_EQ(result.path.back(), grid.at(2, 1));
}

TEST(PathSearchTest, UsesDiagonalRouteWhenAvailable) {
	Grid grid(4, 4, true);
	auto result = grid.find(grid.at(0, 0), grid.at(3, 3));
	ASSERT_EQ(result.path.size(), 3);
	EXPECT_EQ(result.path[0], grid.at(1, 1));
	EXPECT_EQ(result.path[2], grid.at(3, 3));
}
