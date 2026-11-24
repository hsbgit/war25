#pragma once

namespace RuneRules {

inline constexpr int Damage = 50;

enum class TriggerResult { KeepRune, ConsumeRune, DamageAndConsume };

inline TriggerResult triggerResult(bool hasGroundObject, bool objectIsDead,
	bool objectIsPlaced) {
	if (!hasGroundObject) return TriggerResult::KeepRune;
	if (objectIsDead || !objectIsPlaced) return TriggerResult::ConsumeRune;
	return TriggerResult::DamageAndConsume;
}

} // namespace RuneRules
