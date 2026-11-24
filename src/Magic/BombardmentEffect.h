#pragma once

#include "Map/MapObject.h"
#include "Resources/Animation.h"
#include <string>

class Map;

class BombardmentEffect final : public MapObject {
public:
	BombardmentEffect(Object* caster, Map* map, const Point& impactTile,
		const Animation& animation, int damage, int delayMs,
		int travelMs, int fallHeightTiles, int impactFrameStart = -1,
		std::string impactSound = {});

	void process() override;
	bool isExpired() const override { return m_expired; }
	Sprite& getSprite() override { return const_cast<Sprite&>(m_animation.getCurrentFrame()); }
	const Point& getPositionWorld_px() const override { return m_positionWorldPx; }

private:
	void updatePosition();
	void impact();

	Map* m_map;
	Animation m_animation;
	Animation m_impactAnimation;
	Point m_impactTile;
	Point m_positionWorldPx{-100000, -100000};
	int m_damage;
	int m_delayTicks;
	int m_travelTicks;
	int m_elapsedTravelTicks = 0;
	int m_fallHeightTiles;
	std::string m_impactSound;
	bool m_hasImpactAnimation = false;
	bool m_showingImpact = false;
	bool m_impacted = false;
	bool m_expired = false;
};
