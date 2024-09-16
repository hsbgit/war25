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
#include "Stables.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"


Stables::Stables(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 150, 500, 3) {
	setProperties();
}


Stables::Stables(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 500, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Stables::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_STABLES, this->getFraction(), getOwner()->getColor());

	setName((getFraction() == Fraction::Alliance) ? "Stables" : "Ogre Mound?");

	if (getFraction() == Fraction::Alliance) {
		setName("Stables");
		setSoundSelected("human/buildings/stables.wav");
	}
	else {
		setName("Ogre Mound");
		setSoundSelected("orc/buildings/ogre_mound.wav");
	}
}
