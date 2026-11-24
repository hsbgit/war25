#pragma once

#include <algorithm>

namespace AnimationRules {

struct UnitSheetLayout {
	int frameWidth;
	int frameHeight;
	int movingFrames;
	int attackingFrames;
	int dyingFrames;

	constexpr int requiredRows() const {
		return movingFrames + attackingFrames + dyingFrames;
	}

	constexpr bool fits(int textureWidth, int textureHeight,
		int directionColumns = 5) const {
		return frameWidth > 0 && frameHeight > 0 && directionColumns > 0 &&
			frameWidth * directionColumns <= textureWidth &&
			frameHeight * requiredRows() <= textureHeight;
	}
};

inline constexpr UnitSheetLayout HumanArcher{72, 72, 5, 2, 3};
inline constexpr UnitSheetLayout OrcArcher{72, 72, 5, 4, 3};
inline constexpr UnitSheetLayout HumanDemolition{56, 56, 2, 3, 8};
inline constexpr UnitSheetLayout OrcDemolition{56, 56, 2, 3, 10};
inline constexpr UnitSheetLayout GryphonRider{80, 80, 4, 3, 6};
inline constexpr UnitSheetLayout Dragon{88, 80, 4, 1, 5};
inline constexpr UnitSheetLayout HumanMage{72, 72, 5, 4, 7};
inline constexpr UnitSheetLayout DeathKnight{72, 72, 5, 3, 5};
inline constexpr UnitSheetLayout FlyingMachine{80, 80, 2, 0, 2};
inline constexpr UnitSheetLayout Zeppelin{72, 72, 1, 0, 0};

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
