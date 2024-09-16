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

#include "Building.h"


class Farm : public Building {
public:
	Farm(Peasant* pErecter, Map* pMap);
	Farm(Player* pOwner, Fraction fraction, Map* pMap, BuildingState state = BuildingState::Constructed);
	~Farm();

protected:
	void onConstructionFinished() override;

private:
	void setProperties();
};
