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
#include <set>
#include <vector>
#include <cassert>
#include "AttackerStats.h"


class Object;


class IArmed {
public:
	IArmed();
	IArmed(int basicDamage, int piercingDamage, int range, int attackCooldownTime_ticks = 0, int attackHeatupTime_ticks = 0);

	inline AttackerStats getAttackerStats() const {
		return AttackerStats(getBasicDamage(), getBasicPiercingDamage());
	}

	std::set<Object*> findTargetObject(Object* pAttacker) const;
	bool hasTargetInRange(Object* pAttacker) const;
	void setArmedProperties(int basicDamage, int piercingDamage, int range, int attackCooldownTime_ticks = 0, int attackHeatupTime_ticks = 0);


	// Special case: Towers also implements IArmed. However, they are not armed until upgraded (they use 'setArmedProperties()' for this)
	inline bool isArmed() const {
		return (m_basicDamage || m_piercingDamage);
	}

	virtual bool canAttackAirUnits() const {
		return false;
	}

	inline int getBasicDamage() const {
		return m_basicDamage;
	}

	inline int getBasicPiercingDamage() const {
		return m_piercingDamage;
	}

	inline int getPiercingDamage() const {
		return m_piercingDamage + getAdditionalPiercingDamage();
	}

	inline int attackRange() const {
		return m_range + getAdditionalAttackRange();
	}

	inline bool isCloseRangeUnit() const {
		return (m_range > 1) ? false : true;
	}

	inline void releaseActiveTarget() {
		assert(m_pActiveTarget);
		m_pActiveTarget = nullptr;
	}


protected:
	virtual int getAdditionalPiercingDamage() const = 0;
	virtual int getAdditionalAttackRange() const = 0;

	int m_AttackHeatupTime_ticks = 0;
	int m_AttackCooldownTime_ticks = 0;

	bool hasActiveTarget() const;
	void setActiveTarget(Object* pActiveTarget);
	Object* m_pActiveTarget = nullptr;

private:
	int m_basicDamage, m_piercingDamage;
	int m_range;
};
