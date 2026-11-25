#pragma once

#include <cstddef>
#include <pud.h>

namespace PudMapRules {

inline bool isStartLocation(const Pud_Unit_Info& unit) {
	return unit.type == PUD_UNIT_HUMAN_START || unit.type == PUD_UNIT_ORC_START;
}

// Classic melee PUDs contain neutral resources and one start marker per
// player, but no explicit workers. Scenario maps with pre-placed player units
// already define their own starting forces and must not receive extras.
inline bool shouldCreateStartingWorkers(const Pud_Unit_Info* units, std::size_t count) {
	bool hasStartLocation = false;
	for (std::size_t i = 0; i < count; ++i) {
		if (isStartLocation(units[i])) {
			hasStartLocation = true;
			continue;
		}
		if (units[i].player != PUD_PLAYER_NEUTRAL)
			return false;
	}
	return hasStartLocation;
}

}
