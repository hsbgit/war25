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
#include "Ballista.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"
#include "Global/GlobalAttributes.h"
#include "Map/Map.h"
#include "Units/Projectile.h"
#include "Units/CombatRules.h"
#include <algorithm>


Ballista::Ballista(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 110, 0, 9, 5), IArmed(80, 0, CombatRules::SiegeAttackRangeTiles) {
	if (m_fraction == Fraction::Alliance)
		setName("Ballista");
	else
		setName("Catapult");

	setDyingSound("misc/explosion.wav");
	m_vecSoundsSelected.push_back("units/catapult-ballista/acknowledgement/1.wav");
	m_vecSoundsAcknowledge.push_back("units/catapult-ballista/acknowledgement/1.wav");

	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Ballista_1);
	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Ballista_2);

	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_BALLISTA, this->getFraction(), getOwner()->getColor());
}


Object* Ballista::selectTarget() const {
	std::set<Object*> targets = findTargetObject(const_cast<Ballista*>(this));
	if (targets.empty())
		return nullptr;
	return *std::min_element(targets.begin(), targets.end(), [](Object* lhs, Object* rhs) {
		auto* lhsProcessor = dynamic_cast<EventProcessor*>(lhs);
		auto* rhsProcessor = dynamic_cast<EventProcessor*>(rhs);
		return lhsProcessor->getID() < rhsProcessor->getID();
	});
}


Direction Ballista::targetDirection(const Object* target) const {
	const Tile* ownTL = getTileTopLeft();
	const Tile* ownLR = getTileLowerRight();
	const Tile* targetTL = target->getTileTopLeft();
	const Tile* targetLR = target->getTileLowerRight();
	const Point ownCenter = {
		ownTL->getX() + ownLR->getX() + 1,
		ownTL->getY() + ownLR->getY() + 1
	};
	const Point targetCenter = {
		targetTL->getX() + targetLR->getX() + 1,
		targetTL->getY() + targetLR->getY() + 1
	};
	return getTargetCardinalDirection(ownCenter, targetCenter);
}


void Ballista::setAttackFrame(std::size_t frame) {
	Animation attack = g_pRessourceManager->getUnitAnimation(
		this, m_attackDirection, ResourceManager_wartools::AnimationType::Attacking);
	setCurrentAnimation(attack.singleFrame(frame));
}


void Ballista::setReleasedAnimation() {
	Animation attack = g_pRessourceManager->getUnitAnimation(
		this, m_attackDirection, ResourceManager_wartools::AnimationType::Attacking);
	setCurrentAnimation(attack.singleFrame(attack.frameCount() - 1));
}


void Ballista::cancelWindupForMovement(const Event& event) {
	m_windupTicksRemaining = 0;
	m_attackCooldownTicksRemaining = 0;
	clearActiveTarget();
	setReleasedAnimation(); // The bolt/rock is visibly removed during the delay.
	m_pendingMovement = event;
	m_movementDelayTicksRemaining = calculateRequiredTicksForGameAction(3 * 1000);
}


void Ballista::handleEvent(const Event& event) {
	if ((GeneralActions)event.getEventID() == GeneralActions::Move &&
		m_windupTicksRemaining > 0) {
		cancelWindupForMovement(event);
		return;
	}
	if (m_movementDelayTicksRemaining > 0) {
		if ((GeneralActions)event.getEventID() == GeneralActions::Move)
			m_pendingMovement = event;
		else if ((GeneralActions)event.getEventID() == GeneralActions::Stop)
			m_pendingMovement.reset();
		return;
	}
	Unit::handleEvent(event);
}


void Ballista::updateAttack() {
	if (isDead() || !isPlacedOnMap() || isMoving()) {
		m_windupTicksRemaining = 0;
		clearActiveTarget();
		return;
	}
	if (m_attackCooldownTicksRemaining > 0) {
		if (--m_attackCooldownTicksRemaining == 0)
			setIdleAnimation(m_attackDirection);
		return;
	}
	if (hasActiveTarget() && !isActiveTargetInRange(this)) {
		m_windupTicksRemaining = 0;
		clearActiveTarget();
		setIdleAnimation(m_attackDirection);
	}
	if (!hasActiveTarget()) {
		Object* target = selectTarget();
		if (!target)
			return;
		setActiveTarget(target);
		m_attackDirection = targetDirection(target);
		if (getFraction() == Fraction::Orc) {
			// Catapults release immediately; unlike a ballista they have no
			// separate two-second spanning phase.
			fireAtActiveTarget();
		} else {
			// Hold a single spanning frame. Looping both attack frames during
			// the delay made the ballista visibly jump before every shot.
			setAttackFrame(0);
			m_windupTicksRemaining = calculateRequiredTicksForGameAction(2 * 1000);
		}
		return;
	}
	if (m_windupTicksRemaining == 0) {
		m_attackDirection = targetDirection(m_pActiveTarget);
		if (getFraction() == Fraction::Orc) {
			fireAtActiveTarget();
		} else {
			setAttackFrame(0);
			m_windupTicksRemaining = calculateRequiredTicksForGameAction(2 * 1000);
		}
		return;
	}
	if (--m_windupTicksRemaining > 0)
		return;
	fireAtActiveTarget();
}


void Ballista::fireAtActiveTarget() {
	if (!hasActiveTarget())
		return;
	const auto missile = getFraction() == Fraction::Alliance
		? ResourceManager_wartools::MissileType::BallistaBolt
		: ResourceManager_wartools::MissileType::CatapultRock;
	getMap()->addMapObject(new Projectile(missile, this, getMap(), m_pActiveTarget));
	clearActiveTarget();
	setReleasedAnimation();
	const int cooldown = calculateRequiredTicksForGameAction(3 * 1000);
	if (getFraction() == Fraction::Orc) {
		m_attackCooldownTicksRemaining = 0;
		m_movementDelayTicksRemaining = cooldown;
	} else {
		m_attackCooldownTicksRemaining = cooldown;
	}
}


void Ballista::process() {
	Unit::process();
	if (m_movementDelayTicksRemaining > 0) {
		--m_movementDelayTicksRemaining;
		if (m_movementDelayTicksRemaining == 0 && m_pendingMovement) {
			Event movement = *m_pendingMovement;
			m_pendingMovement.reset();
			Unit::handleEvent(movement);
		}
		else if (m_movementDelayTicksRemaining == 0) {
			setIdleAnimation(m_attackDirection);
		}
		return;
	}
	updateAttack();
}
