#pragma once

#include <algorithm>

namespace AnimationRules {

inline constexpr int PaletteCycleFirstIndex = 240;
inline constexpr int PaletteCycleColorCount = 5;
inline constexpr int PaletteCycleFrames = 5;

inline int paletteCycleSourceIndex(int outputIndex, int frame) {
	const int relative = outputIndex - PaletteCycleFirstIndex;
	if (relative < 0 || relative >= PaletteCycleColorCount)
		return outputIndex;
	const int normalizedFrame = ((frame % PaletteCycleFrames) + PaletteCycleFrames) % PaletteCycleFrames;
	return PaletteCycleFirstIndex + (relative + normalizedFrame) % PaletteCycleColorCount;
}

inline int gridFrameCount(int textureWidth, int textureHeight,
	int frameWidth, int frameHeight, int maximumFrames = -1) {
	if (textureWidth <= 0 || textureHeight <= 0 || frameWidth <= 0 || frameHeight <= 0)
		return 0;
	const int available = (textureWidth / frameWidth) * (textureHeight / frameHeight);
	return maximumFrames < 0 ? available : std::min(available, maximumFrames);
}

} // namespace AnimationRules
