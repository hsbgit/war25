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


IArmed::IArmed() : m_basicDamage(0), m_piercingDamage(0), m_range(0) {

}


IArmed::IArmed(int basicDamage, int piercingDamage, int range, int attackCooldownTime_ticks, int attackHeatupTime_ticks) : m_basicDamage(basicDamage), m_piercingDamage(piercingDamage), m_range(range), m_AttackCooldownTime_ticks(), m_AttackHeatupTime_ticks(attackHeatupTime_ticks) {
	setArmedProperties(basicDamage, piercingDamage, range, attackCooldownTime_ticks, attackHeatupTime_ticks);
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
		[pAttacker](Object* pTarget) {
			return !pTarget->isEnemy(pAttacker) || pTarget->getFraction() == Fraction::Neutral;
		});

	return setObjectsInRange;
}


bool IArmed::hasTargetInRange(Object* pAttacker) const {
	return findTargetObject(pAttacker).size();
}


bool IArmed::hasActiveTarget() const {
	// Todo: further checks (e.g., target still in range?)

	// An Object that is currently not placed on a map cant be a target (not visible, e.g., ship in oilrig)
	return (m_pActiveTarget != nullptr && m_pActiveTarget->isPlacedOnMap());
}


void IArmed::setActiveTarget(Object* pActiveTarget) {
	m_pActiveTarget = pActiveTarget;
	m_pActiveTarget->registerAttacker(this); // ToDo: Move to IArmed?
}
