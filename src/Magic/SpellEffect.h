#pragma once

#include "Map/MapObject.h"
#include "Resources/Animation.h"

class Map;
class Unit;

class SpellEffect final : public MapObject {
public:
	enum class Motion { Fixed, FollowTarget, Whirlwind };

	SpellEffect(Object* caster, Map* map, const Point& centerTile,
		const Animation& animation, int durationMs, int damagePerPulse,
		int radiusTiles, Motion motion = Motion::Fixed, Unit* target = nullptr);

	void process() override;
	bool isExpired() const override { return m_remainingTicks <= 0; }
	Sprite& getSprite() override { return const_cast<Sprite&>(m_animation.getCurrentFrame()); }
	const Point& getPositionWorld_px() const override { return m_positionWorldPx; }

private:
	void updatePosition();
	void applyDamage();

	Map* m_map;
	Unit* m_target;
	Animation m_animation;
	Point m_centerTile;
	Point m_positionWorldPx;
	Motion m_motion;
	int m_remainingTicks;
	int m_pulseTicks;
	int m_ticksUntilPulse = 0;
	int m_damagePerPulse;
	int m_radiusTiles;
	int m_motionStep = 0;
};
