#pragma once

#include "Map/MapObject.h"
#include "Resources/Animation.h"

class Map;

class BombardmentEffect final : public MapObject {
public:
	BombardmentEffect(Object* caster, Map* map, const Point& impactTile,
		const Animation& animation, int damage, int delayMs,
		int travelMs, int fallHeightTiles);

	void process() override;
	bool isExpired() const override { return m_expired; }
	Sprite& getSprite() override { return const_cast<Sprite&>(m_animation.getCurrentFrame()); }
	const Point& getPositionWorld_px() const override { return m_positionWorldPx; }

private:
	void updatePosition();
	void impact();

	Map* m_map;
	Animation m_animation;
	Point m_impactTile;
	Point m_positionWorldPx{-100000, -100000};
	int m_damage;
	int m_delayTicks;
	int m_travelTicks;
	int m_elapsedTravelTicks = 0;
	int m_fallHeightTiles;
	bool m_impacted = false;
	bool m_expired = false;
};
