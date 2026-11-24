#pragma once

#include <string>
#include <unordered_set>

namespace AudioRules {

inline bool registerGameplaySound(std::unordered_set<std::string>& playedThisFrame,
	const std::string& soundPath) {
	return playedThisFrame.insert(soundPath).second;
}

} // namespace AudioRules
