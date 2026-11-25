#pragma once

namespace FoodSupplyRules {

// Food capacity is a state contribution: construction grants it once and
// destruction/cancellation removes it only if it had previously been granted.
inline int contributionChange(bool wasProviding, bool willProvide, int capacity) {
	if (wasProviding == willProvide)
		return 0;
	return willProvide ? capacity : -capacity;
}

} // namespace FoodSupplyRules
