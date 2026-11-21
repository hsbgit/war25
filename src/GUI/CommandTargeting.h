#pragma once

#include <unordered_set>
#include <vector>

namespace gui {

template<typename PointType, typename CanPlace>
PointType chooseCommandDestination(
	const PointType& requested,
	const std::vector<PointType>& candidates,
	bool preserveExactTarget,
	int footprint,
	int mapSideLength,
	std::unordered_set<int>& reserved,
	CanPlace canPlace) {
	if (preserveExactTarget)
		return requested;

	for (const PointType& candidate : candidates) {
		if (!canPlace(candidate)) continue;
		bool overlaps = false;
		for (int y = candidate.y; y < candidate.y + footprint; ++y)
			for (int x = candidate.x; x < candidate.x + footprint; ++x)
				overlaps = overlaps || reserved.contains(y * mapSideLength + x);
		if (overlaps) continue;
		for (int y = candidate.y; y < candidate.y + footprint; ++y)
			for (int x = candidate.x; x < candidate.x + footprint; ++x)
				reserved.insert(y * mapSideLength + x);
		return candidate;
	}
	return requested;
}

} // namespace gui
