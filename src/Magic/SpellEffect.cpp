#include "SpellEffect.h"

#include "Global/GlobalAttributes.h"
#include "Global/Object.h"
#include "Map/Map.h"
#include "Map/Tile.h"
#include "Player.h"
#include "Units/Unit.h"

#include <algorithm>
#include <cmath>
#include <set>

SpellEffect::SpellEffect(Object* caster, Map* map, const Point& centerTile,
	const Animation& animation, int durationMs, int damagePerPulse,
	int radiusTiles, Motion motion, Unit* target)
	: MapObject(caster), m_map(map), m_target(target), m_animation(animation),
	  m_centerTile(centerTile), m_motion(motion),
	  m_remainingTicks(calculateRequiredTicksForGameAction(durationMs)),
	  m_pulseTicks(calculateRequiredTicksForGameAction(1000)),
	  m_damagePerPulse(damagePerPulse), m_radiusTiles(radiusTiles) {
	updatePosition();
}

void SpellEffect::updatePosition() {
	if (m_motion == Motion::FollowTarget && m_target && m_target->isPlacedOnMap())
		m_centerTile = m_target->getTileLocation();
	const Sprite& sprite = m_animation.getCurrentFrame();
	m_positionWorldPx = {
		m_centerTile.x * Tile::TileWidth_px + (Tile::TileWidth_px - sprite.getWidth()) / 2,
		m_centerTile.y * Tile::TileWidth_px + (Tile::TileWidth_px - sprite.getHeight()) / 2
	};
}

void SpellEffect::applyDamage() {
	std::set<Object*> hit;
	for (const auto& [color, player] : m_map->m_mapPlayer) {
		(void)color;
		if (!player) continue;
		for (Object* object : player->getObjects()) {
			if (!object || object->isDead() || !object->isPlacedOnMap() || object == m_target)
				continue;
			const Point position = object->getTileLocation();
			if (std::max(std::abs(position.x - m_centerTile.x),
				std::abs(position.y - m_centerTile.y)) <= m_radiusTiles)
				hit.insert(object);
		}
	}
	for (Object* object : hit)
		object->takeDamage(AttackerStats(0, m_damagePerPulse), true);
}

void SpellEffect::process() {
	if (isExpired()) return;
	m_animation.process();
	if (m_motion == Motion::Whirlwind && m_ticksUntilPulse == 0) {
		static constexpr Point offsets[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
		const Point offset = offsets[m_motionStep++ % 4];
		const Point next{m_centerTile.x + offset.x, m_centerTile.y + offset.y};
		if (m_map->getTile(next)) m_centerTile = next;
	}
	updatePosition();
	if (m_ticksUntilPulse-- <= 0) {
		applyDamage();
		m_ticksUntilPulse = m_pulseTicks;
	}
	--m_remainingTicks;
}
