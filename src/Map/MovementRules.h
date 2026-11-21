#pragma once

#include <algorithm>
#include <cstdlib>

namespace MovementRules {

inline bool terrainAllows(bool isLandUnit, bool isSeaUnit, bool canDockOnCoast,
	bool tileIsLand, bool tileIsSea, bool tileIsCoast,
	bool tileIsRocks, bool tileIsWood, bool tileIsWall) {
	if (isLandUnit)
		return tileIsLand && !tileIsCoast && !tileIsRocks && !tileIsWood && !tileIsWall;
	if (isSeaUnit)
		return tileIsSea || (canDockOnCoast && tileIsCoast);
	return true;
}

template<typename Position>
inline int octileDistance(const Position& from, const Position& to) {
	const int dx = std::abs(from.x - to.x);
	const int dy = std::abs(from.y - to.y);
	return 10 * std::max(dx, dy) + 4 * std::min(dx, dy);
}

class NavigationProgress {
public:
	template<typename Position>
	void reset(const Position& start, const Position& target) {
		m_bestDistance = octileDistance(start, target);
		m_stepsWithoutProgress = 0;
	}

	template<typename Position>
	void recordStep(const Position& position, const Position& target) {
		const int distance = octileDistance(position, target);
		if (distance < m_bestDistance) {
			m_bestDistance = distance;
			m_stepsWithoutProgress = 0;
		} else {
			++m_stepsWithoutProgress;
		}
	}

	bool shouldUsePathfinding() const { return m_stepsWithoutProgress >= 3; }
	void pathfindingStarted() { m_stepsWithoutProgress = 0; }

private:
	int m_bestDistance = 0;
	int m_stepsWithoutProgress = 0;
};

} // namespace MovementRules
