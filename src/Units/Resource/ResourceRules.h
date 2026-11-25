#pragma once

namespace ResourceRules {

inline bool eligibleAcceptor(bool exists, bool alive, bool placed,
	bool acceptsCarriedResource) {
	return exists && alive && placed && acceptsCarriedResource;
}

inline int lumberDeliveryBonus(bool millExists, bool millAlive,
	bool millUnderConstruction) {
	return millExists && millAlive && !millUnderConstruction ? 25 : 0;
}

inline bool commandBlockedWhileInsideMine(bool placedOnMap,
	bool hasCurrentMine) {
	return !placedOnMap && hasCurrentMine;
}

inline bool tankerDiesWithDestroyedOilRig(bool rigDestroyed,
	bool tankerInside) {
	return rigDestroyed && tankerInside;
}

} // namespace ResourceRules
