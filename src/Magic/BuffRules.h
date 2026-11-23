#pragma once

#include "Units/UnitActions.h"
#include <optional>

namespace BuffRules {

inline std::optional<BuffType> cancelledBy(BuffType applied) {
	switch (applied) {
	case BuffType::Haste: return BuffType::Slow;
	case BuffType::Slow: return BuffType::Haste;
	default: return std::nullopt;
	}
}

} // namespace BuffRules
