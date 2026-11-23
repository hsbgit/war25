#pragma once

#include <cstdint>
#include <optional>
#include <array>

enum class Season : std::uint8_t { Summer, Winter, Wasteland, Swamp };

inline constexpr std::array<Season, 4> AllSeasons = {
	Season::Summer, Season::Swamp, Season::Wasteland, Season::Winter
};

// Some original Warcraft II objects only have a dedicated extracted image in
// one tileset (the runestone, for example). Prefer the requested season, then
// fall back deterministically to an available original image.
template <typename HasSeason>
std::optional<Season> resolveAvailableSeason(Season requested, HasSeason&& hasSeason) {
	if (hasSeason(requested))
		return requested;
	for (Season candidate : AllSeasons)
		if (hasSeason(candidate))
			return candidate;
	return std::nullopt;
}

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
