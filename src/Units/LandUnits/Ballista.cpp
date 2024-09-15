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
#include "Ballista.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"


Ballista::Ballista(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 110, 0, 9, 5), IArmed(80, 0, 8) {
	if (m_fraction == Fraction::Alliance)
		setName("Ballista");
	else
		setName("Catapult");

	setDyingSound("misc/explosion.wav");
	m_vecSoundsSelected.push_back("units/catapult-ballista/acknowledgement/1.wav");
	m_vecSoundsAcknowledge.push_back("units/catapult-ballista/acknowledgement/1.wav");

	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Ballista_1);
	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Ballista_2);

	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_BALLISTA, this->getFraction(), getOwner()->getColor());
}
