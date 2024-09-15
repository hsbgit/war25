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
#include "Magic/IMagical.h"


class Mage : public Unit, public IArmed, public IMagical {
public:
	Mage(Player* pOwner, Fraction fraction, Map* pMap);

	// EventProcessor
	void handleEvent(const Event& event) override;

	// Object
	void process() override;

	Type getType() const override {
		return Type::Land;
	}

	bool isUnDead() const override {
		return (m_fraction == Fraction::Orc);
	}

	// IArmed
	int getAdditionalPiercingDamage() const override {
		return m_upgradeStats.additionalPiercingDamage();
	}

	int getAdditionalAttackRange() const override {
		return m_upgradeStats.additionalRange();
	}

	// Object
	std::vector<Action> getActions() const override;
};
