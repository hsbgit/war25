#pragma once

#include "Fractions.h"
#include <string_view>

namespace ProductionRules {

inline bool cavalryUpgradeResearched(Fraction fraction,
	bool paladinResearched, bool ogreMageResearched) {
	return fraction == Fraction::Alliance ? paladinResearched : ogreMageResearched;
}

inline std::string_view cavalryReadySound(Fraction fraction, bool upgraded) {
	if (fraction == Fraction::Alliance)
		return upgraded ? "human/units/paladin/ready.wav" : "human/units/knight/ready.wav";
	return upgraded ? "orc/units/ogre-mage/ready.wav" : "orc/units/ogre/ready.wav";
}

inline bool shouldRetireUnplacedCompletedUnit(bool completedUnitExists,
	bool placedOnMap) {
	return completedUnitExists && !placedOnMap;
}

} // namespace ProductionRules
