#include "SpellEffect.h"

#include "Global/GlobalAttributes.h"
#include "Global/Object.h"
#include "Map/Map.h"
#include "Map/Tile.h"
#include "Player.h"
#include "Magic/SpellRules.h"
#include "Resources/SoundManager.h"
#include "Units/Unit.h"

#include <algorithm>
#include <cmath>
#include <set>

SpellEffect::SpellEffect(Object* caster, Map* map, const Point& centerTile,
	const Animation& animation, int durationMs, int damagePerPulse,
	int radiusTiles, Motion motion, Unit* target, bool casterImmune,
	int pulseIntervalMs, int orbitPhase, int periodicSoundIntervalMs,
	const char* periodicSound)
	: MapObject(caster), m_map(map), m_target(target), m_animation(animation),
	  m_centerTile(centerTile), m_originTile(centerTile), m_motion(motion),
	  m_remainingTicks(calculateRequiredTicksForGameAction(durationMs)),
	  m_pulseTicks(calculateRequiredTicksForGameAction(pulseIntervalMs)),
	  m_damagePerPulse(damagePerPulse), m_radiusTiles(radiusTiles),
	  m_casterImmune(casterImmune), m_orbitPhase(orbitPhase),
	  m_periodicSoundTicks(periodicSoundIntervalMs > 0
		? calculateRequiredTicksForGameAction(periodicSoundIntervalMs) : 0),
	  m_periodicSound(periodicSound) {
	updatePosition();
}

void SpellEffect::updatePosition() {
	if ((m_motion == Motion::FollowTarget || m_motion == Motion::OrbitTarget) &&
		m_target && m_target->isPlacedOnMap())
		m_centerTile = m_target->getTileLocation();
	const Sprite& sprite = m_animation.getCurrentFrame();
	if (m_motion == Motion::Whirlwind) {
		const int elapsedMs = m_elapsedTicks * g_tick_ms;
		auto ground = SpellRules::whirlwindGroundPixel(
			{m_originTile.x, m_originTile.y}, elapsedMs, Tile::TileWidth_px);
		ground.x = std::clamp(ground.x, Tile::TileWidth_px / 2,
			m_map->sideLength() * Tile::TileWidth_px - Tile::TileWidth_px / 2);
		ground.y = std::clamp(ground.y, Tile::TileWidth_px / 2,
			m_map->sideLength() * Tile::TileWidth_px - Tile::TileWidth_px / 2);
		m_centerTile = {ground.x / Tile::TileWidth_px, ground.y / Tile::TileWidth_px};
		m_positionWorldPx = {ground.x - sprite.getWidth() / 2,
			ground.y - sprite.getHeight()};
		return;
	}
	if (m_motion == Motion::OrbitTarget) {
		constexpr double Pi = 3.14159265358979323846;
		const double angle = (2.0 * Pi * m_orbitPhase / SpellRules::FlameShieldFlames) +
			(2.0 * Pi * ((m_elapsedTicks * g_tick_ms) % 1600) / 1600.0);
		const int centerX = m_centerTile.x * Tile::TileWidth_px + Tile::TileWidth_px / 2;
		const int centerY = m_centerTile.y * Tile::TileWidth_px + Tile::TileWidth_px / 2;
		constexpr int radiusPx = 24;
		m_positionWorldPx = {
			centerX + static_cast<int>(std::lround(std::cos(angle) * radiusPx)) - sprite.getWidth() / 2,
			centerY + static_cast<int>(std::lround(std::sin(angle) * radiusPx)) - sprite.getHeight() / 2
		};
		return;
	}
	m_positionWorldPx = {
		m_centerTile.x * Tile::TileWidth_px + (Tile::TileWidth_px - sprite.getWidth()) / 2,
		m_centerTile.y * Tile::TileWidth_px + (Tile::TileWidth_px - sprite.getHeight()) / 2
	};
}

void SpellEffect::applyDamage() {
	if (m_damagePerPulse <= 0) return;
	std::set<Object*> hit;
	for (const auto& [color, player] : m_map->m_mapPlayer) {
		(void)color;
		if (!player) continue;
		for (Object* object : player->getObjects()) {
			if (!object || object->isDead() || !object->isPlacedOnMap() || object == m_target ||
				(m_casterImmune && object == m_pSource))
				continue;
			const bool touchesArea = std::any_of(object->getOccupiedTiles().begin(),
				object->getOccupiedTiles().end(), [this](const Tile* tile) {
					return std::max(std::abs(tile->getX() - m_centerTile.x),
						std::abs(tile->getY() - m_centerTile.y)) <= m_radiusTiles;
				});
			if (touchesArea)
				hit.insert(object);
		}
	}
	for (Object* object : hit)
		object->takeDamage(AttackerStats(0, m_damagePerPulse), true);
}

void SpellEffect::process() {
	if (isExpired()) return;
	if ((m_motion == Motion::FollowTarget || m_motion == Motion::OrbitTarget) &&
		(!m_target || m_target->isDead() || !m_target->isPlacedOnMap())) {
		m_remainingTicks = 0;
		m_target = nullptr;
		return;
	}
	m_animation.process();
	++m_elapsedTicks;
	updatePosition();
	if (m_periodicSoundTicks > 0 && m_periodicSound &&
		m_elapsedTicks % m_periodicSoundTicks == 0)
		g_pSoundManager->playWAV(m_periodicSound);
	if (m_ticksUntilPulse-- <= 0) {
		applyDamage();
		m_ticksUntilPulse = std::max(0, m_pulseTicks - 1);
	}
	--m_remainingTicks;
}
