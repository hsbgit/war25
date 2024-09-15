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
#include "Projectile.h"
#include "Resources/SoundManager.h"
#include "Global/Object.h"
#include "Global/Viewport.h"
#include "Global/GlobalAttributes.h"


Projectile::Projectile(ResourceManager_wartools::MissileType type, Object* pSource, Map* pMap, Object* pTarget, const Point& spriteOffset_px)
	: MapObject(pSource),
	m_type(type),
	m_sprite(g_pRessourceManager->getMissile(type)),
	m_attackerStats(dynamic_cast<IArmed*>(pSource)->getAttackerStats()),
	m_expired(false) {

	//m_sprite.setOffset(spriteOffset_px);
	initializeMissile(pSource, pTarget);
}


void Projectile::process() {
	if (m_expired) return;

	updatePosition();
	checkCollision();
}


void Projectile::initializeMissile(Object* pSource, Object* pTarget) {
	if (m_type == ResourceManager_wartools::MissileType::Arrow) {
		g_pSoundManager->playWAV(this, "missiles/bow_throw.wav");
	}

	m_attackedID = dynamic_cast<EventProcessor*>(pTarget)->getID();

	// todo: absturz war, da target nicht mehr on map -> peasant ist in mine gegangen!
	m_originPosition = gViewport.tileToPixel(pSource->getTile()->getPos());
	m_targetPosition = gViewport.tileToPixel(pTarget->getTile()->getPos());


	m_travelDistanceX = std::abs(m_targetPosition.x - m_originPosition.x);
	m_travelDistanceY = std::abs(m_targetPosition.y - m_originPosition.y);

	m_speedX = 1; // Geschwindigkeit in X-Richtung
	m_speedY = 1; // Geschwindigkeit in Y-Richtung

	int distance = static_cast<int>(std::sqrt(m_travelDistanceX * m_travelDistanceX + m_travelDistanceY * m_travelDistanceY));
	if (distance != 0) {
		m_dx_px = (m_targetPosition.x - m_originPosition.x) * 1000 / distance;
		m_dy_px = (m_targetPosition.y - m_originPosition.y) * 1000 / distance;
	}
}


void Projectile::updatePosition() {
	int dt_ms = g_tick_ms; // ToDo: Fix? Better?
	int dt_dist_x_px = (m_speedX * dt_ms * m_dx_px) / 1000;
	int dt_dist_y_px = (m_speedY * dt_ms * m_dy_px) / 1000;

	m_originPosition.x += dt_dist_x_px;
	m_originPosition.y += dt_dist_y_px;

	m_travelDistanceX -= std::abs(dt_dist_x_px);
	m_travelDistanceY -= std::abs(dt_dist_y_px);

	clampToZero(m_travelDistanceX);
	clampToZero(m_travelDistanceY);
}


void Projectile::checkCollision() {
	if (m_travelDistanceX == 0 && m_travelDistanceY == 0) {
		handleCollision();
		m_expired = true;
	}
}


void Projectile::handleCollision() {
	playCollisionSound();

	EventProcessor* pAttackedObj = g_pEventBroker.getEventHandler(m_attackedID);
	Object* pObj = dynamic_cast<Object*>(pAttackedObj);

	// Rare case: Peasant my enter a mine while the arrow is under way. Thus, we have to check whether the object is currently placed on the map
	if (pObj && pObj->isPlacedOnMap()) {
		pObj->takeDamage(m_attackerStats);
	}
	
	// Additional logic for collision
}


void Projectile::playCollisionSound() {
	switch (m_type) {
	case ResourceManager_wartools::MissileType::Arrow:
		g_pSoundManager->playWAV(this, "missiles/bow_hit.wav");
		break;

	case ResourceManager_wartools::MissileType::BallistaBolt:
		[[fallthrough]];
	case ResourceManager_wartools::MissileType::CatapultRock:
		[[fallthrough]];
	case ResourceManager_wartools::MissileType::Cannon:
		g_pSoundManager->playWAV(this, "misc/explosion.wav");
		break;

	default:
		assert(false); // Unknown projectile
		break;
	}
}
