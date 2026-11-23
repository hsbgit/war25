#pragma once

#include <string>
#include <string_view>

namespace UnitPresentationRules {

enum class AttackSound {
	None,
	Sword,
	Punch,
	Bow,
	Axe,
	MageFireball,
	DeathKnight
};

enum class SelectionSound { Selected, Annoyed };

inline SelectionSound selectionSoundForClick(int clickCount, bool hasAnnoyedSounds) {
	return hasAnnoyedSounds && clickCount >= 5
		? SelectionSound::Annoyed : SelectionSound::Selected;
}

inline std::string_view fixedAttackSound(AttackSound sound) {
	switch (sound) {
	case AttackSound::Punch: return "missiles/punch.wav";
	case AttackSound::Bow: return "missiles/bow_throw.wav";
	case AttackSound::Axe: return "missiles/axe_throw.wav";
	case AttackSound::MageFireball: return "missiles/fireball_hit.wav";
	case AttackSound::DeathKnight: return "spells/touch_of_darkness.wav";
	default: return {};
	}
}

inline std::string swordAttackSound(int variant) {
	if (variant < 1) variant = 1;
	if (variant > 3) variant = 3;
	return "missiles/sword_attack/" + std::to_string(variant) + ".wav";
}

inline bool shouldReturnToIdle(bool attackAnimationActive, bool animationComplete) {
	return attackAnimationActive && animationComplete;
}

inline bool supportsAttackGround(bool isDragonOrGryphon) {
	return isDragonOrGryphon;
}

} // namespace UnitPresentationRules
