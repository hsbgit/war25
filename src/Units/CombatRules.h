#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>

namespace CombatRules {

inline constexpr int SiegeAttackRangeTiles = 8;

inline int rectangleDistanceSquared(int leftA, int topA, int rightA, int bottomA,
	int leftB, int topB, int rightB, int bottomB) {
	const int dx = std::max({leftA - rightB, leftB - rightA, 0});
	const int dy = std::max({topA - bottomB, topB - bottomA, 0});
	return dx * dx + dy * dy;
}

// Stratagus compares the integer (floor) map distance with the weapon range.
// Consequently a diagonal whose exact Euclidean distance is slightly larger
// than the range can still be in range (for example 3/3 at range 4).
inline bool rectanglesWithinMapRange(int leftA, int topA, int rightA, int bottomA,
	int leftB, int topB, int rightB, int bottomB, int range) {
	if (range < 0) return false;
	const int distanceSquared = rectangleDistanceSquared(
		leftA, topA, rightA, bottomA, leftB, topB, rightB, bottomB);
	const int nextDistance = range + 1;
	return distanceSquared < nextDistance * nextDistance;
}

struct AutoTargetPriority {
	bool attackingUs;
	bool canFightBack;
	bool mobile;
	bool alreadyInRange;
	int distanceSquared;
	std::uint64_t stableId;
};

inline bool isBetterAutoTarget(const AutoTargetPriority& candidate,
	const AutoTargetPriority& current) {
	if (candidate.attackingUs != current.attackingUs)
		return candidate.attackingUs;
	if (candidate.canFightBack != current.canFightBack)
		return candidate.canFightBack;
	if (candidate.mobile != current.mobile)
		return candidate.mobile;
	if (candidate.alreadyInRange != current.alreadyInRange)
		return candidate.alreadyInRange;
	if (candidate.distanceSquared != current.distanceSquared)
		return candidate.distanceSquared < current.distanceSquared;
	return candidate.stableId < current.stableId;
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
