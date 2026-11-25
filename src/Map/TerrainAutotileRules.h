#pragma once

#include <array>
#include <cstdint>

namespace TerrainAutotileRules {

// N, E, S, W, NE, SE, SW, NW. Diagonals are required to distinguish an
// inside corner from a straight forest edge.
inline std::uint8_t neighbourMask(const std::array<bool, 8>& neighbours) {
	std::uint8_t mask = 0;
	for (std::size_t i = 0; i < neighbours.size(); ++i)
		if (neighbours[i]) mask |= static_cast<std::uint8_t>(1u << i);
	return mask;
}

inline int maskDifferenceCost(std::uint8_t wanted, std::uint8_t candidate) {
	const std::uint8_t difference = wanted ^ candidate;
	int cost = 0;
	for (int bit = 0; bit < 8; ++bit) {
		if (difference & static_cast<std::uint8_t>(1u << bit))
			cost += bit < 4 ? 3 : 1;
	}
	return cost;
}

}
