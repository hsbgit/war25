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
#include "Transport.h"
#include "Resources/resourcemanager_wartools.h"
#include "Player.h"


Transport::Transport(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 150, 0, 4, 10) {
	// Generic behavior
	if (m_fraction == Fraction::Alliance) {
		setSoundBaseDirectory("human/ships/");
	}
	else {
		setSoundBaseDirectory("orc/ships/");
	}

	setDyingSound("ships/sinking.wav");

	// Specific behavior
	addAllowedUpgrade(Upgrade::ItemType::Foundry_Ship_Armor_1);
	addAllowedUpgrade(Upgrade::ItemType::Foundry_Ship_Armor_2);

	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_TRANSPORT, this->getFraction(), getOwner()->getColor());
	setName("Transport");
}
