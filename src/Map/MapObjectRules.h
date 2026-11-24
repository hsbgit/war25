#pragma once

namespace MapObjectRules {

inline bool eligibleForMapRendering(bool placedOnMap, bool dead) {
	return placedOnMap && !dead;
}

} // namespace MapObjectRules
