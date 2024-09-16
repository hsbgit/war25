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
#include "OilRig.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"


OilRig::OilRig(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 175, 650, 3) {
	setProperties();
}


OilRig::OilRig(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 650, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void OilRig::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_OIL_WELL, this->getFraction(), getOwner()->getColor()); //PUD_ICON_OIL_PATCH

	setName("Oil Platform");
	setSoundSelected("buildings/oil_platform.wav");
}

