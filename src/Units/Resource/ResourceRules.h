#pragma once

namespace ResourceRules {

inline bool eligibleAcceptor(bool exists, bool alive, bool placed,
	bool acceptsCarriedResource) {
	return exists && alive && placed && acceptsCarriedResource;
}

} // namespace ResourceRules
