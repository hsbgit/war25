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
#include "Units/IArmed.h"


class GnomishSubmarine : public Unit, public IArmed {
public:
	GnomishSubmarine(Player* pOwner, Fraction fraction, Map* pMap);

	Type getType() const override {
		return Type::Sea;
	}

	bool isOrganic() const override {
		return false;
	}

	// IArmed
	// GnomishSubmarine is not affected by upgrading cannons
	// Could be the same code as implemented for Destroyer or Battleship with the same result
	// but this approach clearly shows that GnomishSubmarine never has additional piercing damage
	int getAdditionalPiercingDamage() const override {
		return 0;
	}

	int getAdditionalAttackRange() const override {
		return 0;
	}
};
