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
#include "Global/GlobalAttributes.h"
#include "Map/Map.h"
#include "Player.h"

#include <algorithm>
#include <cmath>


Projectile::Projectile(ResourceManager_wartools::MissileType type, Object* pSource, Map* pMap, Object* pTarget, const Point&)
	: MapObject(pSource),
	m_type(type),
	m_sprite(g_pRessourceManager->getMissile(type)),
	m_attackerStats(dynamic_cast<IArmed*>(pSource)->getAttackerStats()),
	m_pMap(pMap),
	m_pAttackerOwner(pSource->getOwner()),
	m_expired(false) {

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
	} else if (m_type == ResourceManager_wartools::MissileType::Axe ||
		m_type == ResourceManager_wartools::MissileType::GryphonHammer) {
		g_pSoundManager->playWAV(this, "missiles/axe_throw.wav");
	} else if (m_type == ResourceManager_wartools::MissileType::Fireball ||
		m_type == ResourceManager_wartools::MissileType::DragonBreath) {
		g_pSoundManager->playWAV(this, "missiles/fireball_throw.wav");
	} else {
		g_pSoundManager->playWAV(this, "missiles/catapult-ballista_attack.wav");
	}

	m_attackedID = dynamic_cast<EventProcessor*>(pTarget)->getID();
	m_attackerID = dynamic_cast<EventProcessor*>(pSource)->getID();

	if (!pSource->isPlacedOnMap() || !pTarget->isPlacedOnMap()) {
		m_expired = true;
		return;
	}

	auto objectCenter = [](Object* object) {
		const Point& topLeft = object->getTileTopLeft()->getPos();
		const Point& lowerRight = object->getTileLowerRight()->getPos();
		return Point{
			(topLeft.x + lowerRight.x + 1) * Tile::TileWidth_px / 2,
			(topLeft.y + lowerRight.y + 1) * Tile::TileWidth_px / 2
		};
	};
	const Point sourceCenter = objectCenter(pSource);
	const Point targetCenter = objectCenter(pTarget);
	m_impactTile = {targetCenter.x / Tile::TileWidth_px, targetCenter.y / Tile::TileWidth_px};
	const Direction missileDirection = sourceCenter == targetCenter
		? Direction::N
		: getTargetCardinalDirection(sourceCenter, targetCenter);
	m_sprite = g_pRessourceManager->getMissile(m_type, missileDirection);

	// Map objects use absolute world pixels. Viewport conversion here made
	// arrows jump when the camera moved and aimed at the target's top-left.
	m_originPosition = {sourceCenter.x - m_sprite.getWidth() / 2, sourceCenter.y - m_sprite.getHeight() / 2};
	m_targetPosition = {targetCenter.x - m_sprite.getWidth() / 2, targetCenter.y - m_sprite.getHeight() / 2};
	m_startPosition = m_originPosition;

	const int deltaX = m_targetPosition.x - m_originPosition.x;
	const int deltaY = m_targetPosition.y - m_originPosition.y;
	const int distance = static_cast<int>(std::sqrt(deltaX * deltaX + deltaY * deltaY));
	const int pixelsPerTick = std::max(1, g_tick_ms);
	m_totalTravelTicks = std::max(1, (distance + pixelsPerTick - 1) / pixelsPerTick);
}


void Projectile::updatePosition() {
	m_elapsedTravelTicks = std::min(m_elapsedTravelTicks + 1, m_totalTravelTicks);
	m_originPosition.x = m_startPosition.x +
		(m_targetPosition.x - m_startPosition.x) * m_elapsedTravelTicks / m_totalTravelTicks;
	m_originPosition.y = m_startPosition.y +
		(m_targetPosition.y - m_startPosition.y) * m_elapsedTravelTicks / m_totalTravelTicks;
}


void Projectile::checkCollision() {
	if (m_elapsedTravelTicks >= m_totalTravelTicks) {
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
		pObj->onAttackedBy(dynamic_cast<Object*>(g_pEventBroker.getEventHandler(m_attackerID)));
		pObj->takeDamage(m_attackerStats);
	}
	if (m_type == ResourceManager_wartools::MissileType::CatapultRock)
		handleCatapultSplash(pObj);
	
	// Additional logic for collision
}


void Projectile::handleCatapultSplash(Object* primaryTarget) {
	if (!m_pMap || !m_pAttackerOwner)
		return;
	AttackerStats splash(
		std::max(1, m_attackerStats.getBasicDamage() / 2),
		m_attackerStats.getPiercingDamage() / 2);
	for (const auto& [color, player] : m_pMap->m_mapPlayer) {
		if (!player || player == m_pAttackerOwner || player->isNeutral())
			continue;
		for (Object* object : player->getObjects()) {
			if (!object || object == primaryTarget || object->isDead() || !object->isPlacedOnMap())
				continue;
			bool inSplashRadius = false;
			for (Tile* tile : object->getOccupiedTiles()) {
				if (std::abs(tile->getX() - m_impactTile.x) <= 1 &&
					std::abs(tile->getY() - m_impactTile.y) <= 1) {
					inSplashRadius = true;
					break;
				}
			}
			if (inSplashRadius) {
				object->onAttackedBy(dynamic_cast<Object*>(
					g_pEventBroker.getEventHandler(m_attackerID)));
				object->takeDamage(splash);
			}
		}
	}
}


void Projectile::playCollisionSound() {
	switch (m_type) {
	case ResourceManager_wartools::MissileType::Arrow:
		g_pSoundManager->playWAV(this, "missiles/bow_hit.wav");
		break;
	case ResourceManager_wartools::MissileType::Axe:
	case ResourceManager_wartools::MissileType::GryphonHammer:
		g_pSoundManager->playWAV(this, "missiles/punch.wav");
		break;
	case ResourceManager_wartools::MissileType::Fireball:
	case ResourceManager_wartools::MissileType::DragonBreath:
		g_pSoundManager->playWAV(this, "missiles/fireball_hit.wav");
		break;

	case ResourceManager_wartools::MissileType::BallistaBolt:
		[[fallthrough]];
	case ResourceManager_wartools::MissileType::CatapultRock:
		[[fallthrough]];
	case ResourceManager_wartools::MissileType::Cannon:
	case ResourceManager_wartools::MissileType::SubmarineMissile:
	case ResourceManager_wartools::MissileType::TurtleMissile:
		g_pSoundManager->playWAV(this, "misc/explosion.wav");
		break;

	default:
		assert(false); // Unknown projectile
		break;
	}
}
