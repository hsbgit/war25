#pragma once

#include <algorithm>
#include <cstdlib>

namespace CombatRules {

inline constexpr int SiegeAttackRangeTiles = 8;

inline int rectangleDistanceSquared(int leftA, int topA, int rightA, int bottomA,
	int leftB, int topB, int rightB, int bottomB) {
	const int dx = std::max({leftA - rightB, leftB - rightA, 0});
	const int dy = std::max({topA - bottomB, topB - bottomA, 0});
	return dx * dx + dy * dy;
}

struct DirectionVector { int x; int y; };

inline DirectionVector directionVector(int direction) {
	switch (direction) {
	case 0: return {0, -1};
	case 1: return {1, -1};
	case 2: return {1, 0};
	case 3: return {1, 1};
	case 4: return {0, 1};
	case 5: return {-1, 1};
	case 6: return {-1, 0};
	case 7: return {-1, -1};
	}
	return {0, 0};
}

inline int broadsideFacing(int target, int current) {
	const int clockwise = (target + 2) % 8;
	const int counterClockwise = (target + 6) % 8;
	auto turnDistance = [current](int candidate) {
		const int difference = std::abs(candidate - current);
		return std::min(difference, 8 - difference);
	};
	return turnDistance(clockwise) <= turnDistance(counterClockwise)
		? clockwise : counterClockwise;
}

} // namespace CombatRules
