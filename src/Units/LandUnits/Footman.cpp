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
#include "Footman.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"


Footman::Footman(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 60, 2, 4, 10), IArmed(6, 3, 1) {
	if (m_fraction == Fraction::Alliance) {
		setDyingSound("human/basic_voices/dead.wav");
		setSoundBaseDirectory("human/basic_voices/");
		setName("Footman");
	}
	else {
		setDyingSound("orc/basic_voices/dead.wav");
		setSoundBaseDirectory("orc/basic_voices/"); // Grunt and Peon have identical sounds?
		setName("Grunt");
	}

	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Armor_1);
	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Armor_2);

	if (getFraction() == Fraction::Alliance) {
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_1_Human);
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_2_Human);
	}
	else {
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_1_Orc);
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_2_Orc);
	}

	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_FOOTMAN, this->getFraction(), getOwner()->getColor());
}
