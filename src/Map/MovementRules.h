#pragma once

#include <algorithm>
#include <cstdlib>
#include <optional>

namespace MovementRules {

inline bool tileWithinMap(int x, int y, int sideLength) {
	return sideLength > 0 && x >= 0 && y >= 0 && x < sideLength && y < sideLength;
}

inline bool footprintWithinMap(int x, int y, int tileSize, int sideLength) {
	return tileSize > 0 && tileWithinMap(x, y, sideLength) &&
		x <= sideLength - tileSize && y <= sideLength - tileSize;
}

inline bool terrainAllows(bool isLandUnit, bool isSeaUnit, bool canDockOnCoast,
	bool tileIsLand, bool tileIsSea, bool tileIsCoast,
	bool tileIsRocks, bool tileIsWood, bool tileIsWall) {
	if (isLandUnit)
		return tileIsLand && !tileIsCoast && !tileIsRocks && !tileIsWood && !tileIsWall;
	if (isSeaUnit)
		return tileIsSea || (canDockOnCoast && tileIsCoast);
	return true;
}

// Preserve the swept-footprint check used by the working 2x2 air movement.
// Land and sea movement use the classic eight-way grid: when the destination
// footprint is free, a diagonal step is valid. Requiring both orthogonal tiles
// for 1x1 land units can split otherwise connected parts of a Warcraft II map.
inline bool requiresOrthogonalDiagonalClearance(bool isAirUnit) {
	return isAirUnit;
}

inline bool reachedTilePixelOffset(int offset, int speed) {
	return speed == 0 || (speed > 0 ? offset >= 0 : offset <= 0);
}

inline bool shouldShowIdleAnimation(bool hasReservedNextTile,
	bool temporarilyBlocked) {
	return !hasReservedNextTile && temporarilyBlocked;
}

inline std::optional<int> occupancyTraversalCost(bool occupied,
	bool occupiedBySelf, bool occupantIsUnit,
	bool allowTemporaryUnits = true) {
	if (!occupied || occupiedBySelf)
		return 0;
	// Buildings and other stationary map objects can never vacate the tile.
	if (!occupantIsUnit)
		return std::nullopt;
	if (!allowTemporaryUnits)
		return std::nullopt;
	// A large but finite penalty routes around idle units whenever possible,
	// while retaining a route through temporarily busy one-tile chokepoints.
	return 200;
}

inline bool occupantBlocksMovement(bool occupied, bool occupiedBySelf,
	bool walkableGroundOverlay) {
	return occupied && !occupiedBySelf && !walkableGroundOverlay;
}

template<typename Position>
inline int octileDistance(const Position& from, const Position& to) {
	const int dx = std::abs(from.x - to.x);
	const int dy = std::abs(from.y - to.y);
	return 10 * std::max(dx, dy) + 4 * std::min(dx, dy);
}

} // namespace MovementRules
