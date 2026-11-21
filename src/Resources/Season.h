#pragma once

#include <cstdint>
#include <optional>
#include <array>

enum class Season : std::uint8_t { Summer, Winter, Wasteland, Swamp };

inline constexpr std::array<Season, 4> AllSeasons = {
	Season::Summer, Season::Swamp, Season::Wasteland, Season::Winter
};

class SeasonObserver {
public:
	virtual ~SeasonObserver() = default;
	virtual void notifySeasonChanged(Season newSeason) = 0;
};

inline std::optional<Season> seasonForHotkey(char key) {
	switch (key) {
	case 's': return Season::Summer;
	case 'd': return Season::Swamp;
	case 'w': return Season::Winter;
	case 'a': return Season::Wasteland;
	default: return std::nullopt;
	}
}
