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
#include "Units/Resource/IResourceMine.h"

class IResourceCollector;

class GoldMine : public Building, public IResourceMine {
public:
	GoldMine(Player* pOwner, Map* pMap, int gold);

	void process() override;

	bool enter(IResourceCollector* pPeasant) override;
	void die() override;

private:
	bool placePeasant(Peasant* pPeasant, bool withGold);
	void switchImage(bool miningActive) override;
};
