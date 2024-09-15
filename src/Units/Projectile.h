/*
	Copyright (C) 2024 P. Last

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "Map/MapObject.h"
#include "Resources/resourcemanager_wartools.h"
#include "EventHandling/EventBroker.h"

class Object;


class Projectile : public MapObject {
public:
	Projectile(ResourceManager_wartools::MissileType type, Object* pSource, Map* pMap, Object* pTarget, const Point& spriteOffset_px = { 0, 0 });

	void process() override;

	const Point& getPositionWorld_px() const override {
		return m_originPosition;
	}

	// Todo: delete if expired)
	bool isExpired() const override {
		return m_expired;
	}

	Sprite& getSprite() override {
		return m_sprite;
	}

private:
	void initializeMissile(Object* pSource, Object* pTarget);

	void updatePosition();
	void checkCollision();
	void handleCollision();
	void playCollisionSound();

	static void clampToZero(int& value) {
		if (value < 0) value = 0;
	}

private:
	EventProcessorID m_attackedID;
	AttackerStats m_attackerStats;
	ResourceManager_wartools::MissileType m_type;
	Sprite m_sprite;
	int m_speedX;
	int m_speedY;
	int m_dx_px;
	int m_dy_px;
	Point m_targetPosition;
	Point m_originPosition;
	int m_travelDistanceX;
	int m_travelDistanceY;
	bool m_expired = false;
};
