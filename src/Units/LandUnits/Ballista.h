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

#include "Units/Unit.h"
#include <optional>


class Ballista : public Unit, public IArmed {
public:
	Ballista(Player* pOwner, Fraction fraction, Map* pMap);
	void process() override;
	void handleEvent(const Event& event) override;

	Type getType() const override {
		return Type::Land;
	}

	bool isOrganic() const override {
		return false;
	}

	int getAdditionalPiercingDamage() const override {
		return m_upgradeStats.additionalPiercingDamage();
	}

	int getAdditionalAttackRange() const override {
		return m_upgradeStats.additionalRange();
	}

private:
	void updateAttack();
	void fireAtActiveTarget();
	void cancelWindupForMovement(const Event& event);
	void setReleasedAnimation();
	void setAttackFrame(std::size_t frame);
	Object* selectTarget() const;
	Direction targetDirection(const Object* target) const;

	int m_windupTicksRemaining = 0;
	int m_attackCooldownTicksRemaining = 0;
	int m_movementDelayTicksRemaining = 0;
	std::optional<Event> m_pendingMovement;
	Direction m_attackDirection = Direction::N;
};
