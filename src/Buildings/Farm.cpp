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
#include "Farm.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"


Farm::Farm(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 100, 400, 3) {
	setProperties();
}


Farm::Farm(Player* pOwner, Fraction fraction, Map* pMap, BuildingState state) : Building(pOwner, fraction, pMap, 400, 3) {
	setProperties();
	getOwner()->increaseTotalFood(4);

	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Farm::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_FARM, this->getFraction(), getOwner()->getColor());

	if (getFraction() == Fraction::Alliance) {
		setName("Farm");
		setSoundSelected("human/buildings/farm.wav");
	}
	else {
		setName("Pig Farm");
		setSoundSelected("orc/buildings/pig_farm.wav");
	}
}


Farm::~Farm() {
	getOwner()->decreaseTotalFood(4);
}


void Farm::onConstructionFinished() {
	Building::onConstructionFinished();
	getOwner()->increaseTotalFood(4);
}
