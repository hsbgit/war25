#pragma once

#include <array>
#include <algorithm>

namespace SpellRules {

struct TilePoint { int x; int y; };

inline constexpr int BombardmentFields = 5;
inline constexpr int BombardmentShards = 11;
inline constexpr int BlizzardFallHeightTiles = 4;
inline constexpr int BombardmentShardDelayMs = 120;
inline constexpr int BlizzardFallDurationMs = 500;
inline constexpr int BlizzardDebugCycles = 5;
inline constexpr int BlizzardCycleDelayMs = 1500;
inline constexpr int FireballDamage = 34;
inline constexpr int CorpseFreshDurationMs = 4000;
inline constexpr int CorpseRaiseableDurationMs = 3500;
inline constexpr int CorpseTotalDurationMs = 10000;
inline constexpr int WhirlwindDurationMs = 60000;
inline constexpr int WhirlwindSoundIntervalMs = 20000;
inline constexpr int WhirlwindDamageIntervalMs = 200;
inline constexpr int WhirlwindDamagePerPulse = 1;
inline constexpr int WhirlwindMoveSegmentMs = 1000;
inline constexpr int FlameShieldFlames = 6;
inline constexpr int FlameShieldDurationMs = 12000;
inline constexpr int FlameShieldDamagePerPulse = 8;
inline constexpr int RaisedSkeletonCount = 2;
inline constexpr int RaisedSkeletonLifetimeMs = 90000;
inline constexpr int SpellTriggeredDemolitionSplashDamage = 0;

// Wargus uses a five-field area with eleven shards. Keep the pattern
// deterministic for lockstep games while covering the complete 5x5 square.
inline constexpr std::array<TilePoint, BombardmentShards> BombardmentOffsets{{
	{-2, -2}, {0, -2}, {2, -2}, {-1, -1}, {1, -1},
	{-2, 0}, {0, 0}, {2, 0}, {-1, 2}, {1, 2}, {0, 1}
}};

inline constexpr std::array<TilePoint, 9> WhirlwindPath{{
	{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0},
	{-1, 0}, {-1, -1}, {0, -1}, {0, 0}
}};

inline bool corpseIsRaiseable(int elapsedMs) {
	return elapsedMs >= CorpseFreshDurationMs &&
		elapsedMs < CorpseFreshDurationMs + CorpseRaiseableDurationMs;
}

inline int blizzardCycles(bool debugCast) {
	return debugCast ? BlizzardDebugCycles : 1;
}

inline int fallingOffsetPixels(int fallHeightTiles, int tileWidth,
	int elapsedTicks, int travelTicks) {
	return fallHeightTiles * tileWidth *
		std::max(0, travelTicks - elapsedTicks) / std::max(1, travelTicks);
}

inline TilePoint whirlwindGroundPixel(const TilePoint& originTile, int elapsedMs,
	int tileWidth) {
	const int segment = (elapsedMs / WhirlwindMoveSegmentMs) %
		(static_cast<int>(WhirlwindPath.size()) - 1);
	const int within = elapsedMs % WhirlwindMoveSegmentMs;
	const TilePoint from = WhirlwindPath[segment];
	const TilePoint to = WhirlwindPath[segment + 1];
	return {
		originTile.x * tileWidth + tileWidth / 2 +
		((from.x * (WhirlwindMoveSegmentMs - within) + to.x * within) * tileWidth) /
			WhirlwindMoveSegmentMs,
		originTile.y * tileWidth + tileWidth / 2 +
		((from.y * (WhirlwindMoveSegmentMs - within) + to.y * within) * tileWidth) /
			WhirlwindMoveSegmentMs
	};
}

template<typename PointLike>
inline TilePoint bombardmentTile(const PointLike& center, int shardIndex, int mapSideLength) {
	const TilePoint offset = BombardmentOffsets[shardIndex % BombardmentShards];
	return {
		std::clamp(center.x + offset.x, 0, mapSideLength - 1),
		std::clamp(center.y + offset.y, 0, mapSideLength - 1)
	};
}

template<typename PointLike>
inline int bombardmentDamage(int shardIndex, const PointLike& center) {
	// Warcraft II/Wargus uses Rand(10). This equivalent 0..9 sequence is
	// deterministic across clients and varies with cast position and shard.
	const unsigned seed = static_cast<unsigned>(center.x * 31 + center.y * 17 +
		shardIndex * 7);
	return static_cast<int>(seed % 10);
}

template<typename PointLike>
inline TilePoint decayTile(const PointLike& center, int shardIndex, int mapSideLength) {
	// A deterministic permutation keeps lockstep clients identical while making
	// the stationary decay sprites appear throughout the rectangle, not row-wise.
	constexpr std::array<int, BombardmentShards> permutation{{
		6, 0, 9, 3, 10, 1, 8, 5, 2, 7, 4
	}};
	return bombardmentTile(center, permutation[shardIndex % BombardmentShards],
		mapSideLength);
}

inline int decayDelayMs(int shardIndex) {
	constexpr std::array<int, BombardmentShards> delays{{
		0, 480, 120, 840, 360, 960, 240, 720, 600, 1080, 420
	}};
	return delays[shardIndex % BombardmentShards];
}

} // namespace SpellRules
