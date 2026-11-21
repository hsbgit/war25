#pragma once

#include "Units/UnitActions.h"
#include <algorithm>

namespace SpellCosts {

struct ExorcismResult {
	int damage = 0;
	int mana = 0;
};

inline ExorcismResult exorcism(int availableMana, int targetHealth) {
	if (availableMana <= 0 || targetHealth <= 0) return {};
	const int damage = std::min(targetHealth, availableMana / 4);
	return {damage, damage * 4};
}

inline int manaCost(int spell) {
	switch (spell) {
	case static_cast<int>(MageActions::Fireball): return 100;
	case static_cast<int>(BuffType::Slow): return 50;
	case static_cast<int>(MageActions::FlameShield): return 80;
	case static_cast<int>(BuffType::Invisibility): return 200;
	case static_cast<int>(MageActions::Polymorph): return 200;
	case static_cast<int>(MageActions::Blizzard): return 25;
	case static_cast<int>(DeathKnightActions::DeathCoil): return 100;
	case static_cast<int>(BuffType::Haste): return 50;
	case static_cast<int>(DeathKnightActions::RaiseTheDead): return 50;
	case static_cast<int>(DeathKnightActions::Whirlwind): return 100;
	case static_cast<int>(BuffType::UnholyArmor): return 200;
	case static_cast<int>(DeathKnightActions::DeathAndDecay): return 25;
	default: return -1;
	}
}

} // namespace SpellCosts
