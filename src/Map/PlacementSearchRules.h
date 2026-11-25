#pragma once

#include <optional>

namespace PlacementSearchRules {

struct Position {
	int x;
	int y;
	bool operator==(const Position&) const = default;
};

// Search square rings around an origin while asking the caller about the
// complete footprint, not merely the top-left tile.
template<typename IsFootprintAvailable>
std::optional<Position> nearestFreeTopLeft(const Position& origin, int maxDistance,
	int sideLength, int footprintSize, IsFootprintAvailable isAvailable) {
	if (maxDistance < 1 || sideLength < 1 || footprintSize < 1)
		return std::nullopt;
	for (int distance = 1; distance <= maxDistance; ++distance) {
		for (int dy = -distance; dy <= distance; ++dy) {
			for (int dx = -distance; dx <= distance; ++dx) {
				if (dx != -distance && dx != distance &&
					dy != -distance && dy != distance)
					continue;
				const Position candidate{origin.x + dx, origin.y + dy};
				if (candidate.x < 0 || candidate.y < 0 ||
					candidate.x + footprintSize > sideLength ||
					candidate.y + footprintSize > sideLength)
					continue;
				if (isAvailable(candidate.x, candidate.y, footprintSize))
					return candidate;
			}
		}
	}
	return std::nullopt;
}

} // namespace PlacementSearchRules
