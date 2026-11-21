#pragma once

#include <algorithm>

namespace SimulationSettings {
inline constexpr float MinimumSpeed = 0.25f;
inline constexpr float MaximumSpeed = 16.0f;

inline float clampSpeed(float speed) {
	return std::clamp(speed, MinimumSpeed, MaximumSpeed);
}
}
