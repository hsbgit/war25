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
#include "IArmed.h"
#include "Global/Object.h"
#include "Map/Map.h"
#include "Player.h"


IArmed::IArmed() : m_basicDamage(0), m_piercingDamage(0), m_range(0) {

}


IArmed::IArmed(int basicDamage, int piercingDamage, int range, int attackCooldownTime_ticks, int attackHeatupTime_ticks) : m_basicDamage(basicDamage), m_piercingDamage(piercingDamage), m_range(range), m_AttackCooldownTime_ticks(), m_AttackHeatupTime_ticks(attackHeatupTime_ticks) {
	setArmedProperties(basicDamage, piercingDamage, range, attackCooldownTime_ticks, attackHeatupTime_ticks);
}

IArmed::~IArmed() {
	clearActiveTarget();
}


void IArmed::setArmedProperties(int basicDamage, int piercingDamage, int range, int attackCooldownTime_ticks, int attackHeatupTime_ticks) {
	m_basicDamage = basicDamage;
	m_piercingDamage = piercingDamage;
	m_range = range;
	m_AttackCooldownTime_ticks = attackCooldownTime_ticks;
	m_AttackHeatupTime_ticks = attackHeatupTime_ticks;
}


std::set<Object*> IArmed::findTargetObject(Object* pAttacker) const {
	std::set<Object*> setObjectsInRange = pAttacker->getMap()->getVisibleObjectsWithinRange(pAttacker, attackRange(), canAttackAirUnits());

	std::erase_if(setObjectsInRange,
		[pAttacker, this](Object* pTarget) {
			return !pTarget->isEnemy(pAttacker) ||
				pTarget->getFraction() == Fraction::Neutral ||
				pTarget->getOwner()->isNeutral() || !canAttackTarget(pTarget) ||
				!pAttacker->getOwner()->getMapInfo()->canCurrentlySee(pTarget);
		});

	return setObjectsInRange;
}


bool IArmed::hasTargetInRange(Object* pAttacker) const {
	return findTargetObject(pAttacker).size();
}


bool IArmed::hasActiveTarget() const {
	// An Object that is currently not placed on a map cant be a target (not visible, e.g., ship in oilrig)
	return (m_pActiveTarget != nullptr && m_pActiveTarget->isPlacedOnMap());
}

bool IArmed::isActiveTargetInRange(Object* pAttacker) const {
	if (!hasActiveTarget()) return false;
	const auto targets = findTargetObject(pAttacker);
	return targets.contains(m_pActiveTarget);
}

void IArmed::clearActiveTarget() {
	if (!m_pActiveTarget) return;
	m_pActiveTarget->unregisterAttacker(this);
	m_pActiveTarget = nullptr;
}


void IArmed::setActiveTarget(Object* pActiveTarget) {
	if (m_pActiveTarget == pActiveTarget) return;
	clearActiveTarget();
	m_pActiveTarget = pActiveTarget;
	m_pActiveTarget->registerAttacker(this);
}
