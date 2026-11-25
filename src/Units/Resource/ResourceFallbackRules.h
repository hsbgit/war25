#pragma once

namespace ResourceFallbackRules {

inline bool shouldSwitchFromGoldToWood(bool mineExists, bool mineAlive,
	int remainingGold, bool hasNearbyWood) {
	return hasNearbyWood && (!mineExists || !mineAlive || remainingGold <= 0);
}

}
