#pragma once

#include "Units/UnitActions.h"
#include "EventHandling/EventHandlerID.h"
#include <array>
#include <optional>

namespace DebugSpellRules {

inline constexpr std::array<EventID, 18> Spells{
	static_cast<EventID>(MageActions::Fireball),
	static_cast<EventID>(BuffType::Slow),
	static_cast<EventID>(MageActions::FlameShield),
	static_cast<EventID>(BuffType::Invisibility),
	static_cast<EventID>(MageActions::Polymorph),
	static_cast<EventID>(MageActions::Blizzard),
	static_cast<EventID>(DeathKnightActions::DeathAndDecay),
	static_cast<EventID>(BuffType::Haste),
	static_cast<EventID>(DeathKnightActions::RaiseTheDead),
	static_cast<EventID>(DeathKnightActions::Whirlwind),
	static_cast<EventID>(BuffType::UnholyArmor),
	static_cast<EventID>(DeathKnightActions::DeathCoil),
	static_cast<EventID>(PaladinActions::HolyVision),
	static_cast<EventID>(PaladinActions::Heal),
	static_cast<EventID>(PaladinActions::Excorcism),
	static_cast<EventID>(OgreActions::EyeOfKillrogg),
	static_cast<EventID>(BuffType::Bloodlust),
	static_cast<EventID>(OgreActions::Runes)
};

inline std::optional<EventID> spellForButton(int button) {
	if (button < 0 || button >= static_cast<int>(Spells.size()))
		return std::nullopt;
	return Spells[button];
}

} // namespace DebugSpellRules
