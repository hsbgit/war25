#include "BombardmentEffect.h"

#include "Global/GlobalAttributes.h"
#include "Global/Object.h"
#include "Map/Map.h"
#include "Map/Tile.h"
#include "Player.h"
#include "Magic/SpellRules.h"
#include "Resources/SoundManager.h"

#include <algorithm>

BombardmentEffect::BombardmentEffect(Object* caster, Map* map,
	const Point& impactTile, const Animation& animation, int damage,
	int delayMs, int travelMs, int fallHeightTiles, int impactFrameStart,
	std::string impactSound)
	: MapObject(caster), m_map(map), m_animation(animation),
	  m_impactTile(impactTile), m_damage(damage),
	  m_delayTicks(delayMs == 0 ? 0 : calculateRequiredTicksForGameAction(delayMs)),
	  m_travelTicks(calculateRequiredTicksForGameAction(std::max(1, travelMs))),
	  m_fallHeightTiles(fallHeightTiles), m_impactSound(std::move(impactSound)) {
	if (impactFrameStart > 0 &&
		static_cast<std::size_t>(impactFrameStart) < animation.frameCount()) {
		m_animation = animation.frameRange(0, static_cast<std::size_t>(impactFrameStart));
		m_impactAnimation = animation.frameRange(static_cast<std::size_t>(impactFrameStart),
			animation.frameCount() - static_cast<std::size_t>(impactFrameStart), true);
		m_hasImpactAnimation = true;
	}
	updatePosition();
}

void BombardmentEffect::updatePosition() {
	if (m_delayTicks > 0) {
		m_positionWorldPx = {-100000, -100000};
		return;
	}
	const Sprite& sprite = m_animation.getCurrentFrame();
	const int remainingFall = SpellRules::fallingOffsetPixels(
		m_fallHeightTiles, Tile::TileWidth_px, m_elapsedTravelTicks, m_travelTicks);
	m_positionWorldPx = {
		m_impactTile.x * Tile::TileWidth_px + (Tile::TileWidth_px - sprite.getWidth()) / 2 - remainingFall,
		m_impactTile.y * Tile::TileWidth_px + (Tile::TileWidth_px - sprite.getHeight()) / 2 - remainingFall
	};
}

void BombardmentEffect::impact() {
	if (m_impacted) return;
	m_impacted = true;
	if (!m_impactSound.empty())
		g_pSoundManager->playWAV(this, m_impactSound);
	for (const auto& [color, player] : m_map->m_mapPlayer) {
		(void)color;
		if (!player) continue;
		for (Object* object : player->getObjects()) {
			if (!object || object->isDead() || !object->isPlacedOnMap()) continue;
			const bool occupiesImpactTile = std::any_of(
				object->getOccupiedTiles().begin(), object->getOccupiedTiles().end(),
				[this](const Tile* tile) {
					return tile->getX() == m_impactTile.x && tile->getY() == m_impactTile.y;
				});
			if (occupiesImpactTile && m_damage > 0)
				object->takeDamage(AttackerStats(0, m_damage), true);
		}
	}
}

void BombardmentEffect::process() {
	if (m_expired) return;
	if (m_delayTicks > 0) {
		--m_delayTicks;
		updatePosition();
		return;
	}
	if (m_showingImpact) {
		m_animation.process();
		if (m_animation.isComplete()) m_expired = true;
		return;
	}
	m_animation.process();
	if (m_elapsedTravelTicks < m_travelTicks) ++m_elapsedTravelTicks;
	updatePosition();
	if (m_elapsedTravelTicks >= m_travelTicks) {
		impact();
		if (m_hasImpactAnimation) {
			m_animation = m_impactAnimation;
			m_showingImpact = true;
			updatePosition();
		} else {
			m_expired = true;
		}
	}
}
