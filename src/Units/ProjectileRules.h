#pragma once

#include <algorithm>

namespace ProjectileRules {

struct SplashProfile {
	int radiusTiles = 0;
	int damageDivisor = 1;

	constexpr bool enabled() const { return radiusTiles > 0; }
};

inline constexpr SplashProfile SiegeSplash{2, 4};
inline constexpr SplashProfile SmallCannonSplash{2, 3};

inline int animationStage(int elapsedTicks, int totalTicks, int stages, bool playOnce) {
	if (stages <= 1) return 0;
	if (playOnce)
		return std::clamp(stages * elapsedTicks / std::max(1, totalTicks), 0, stages - 1);
	return std::max(0, elapsedTicks - 1) % stages;
}

} // namespace ProjectileRules
