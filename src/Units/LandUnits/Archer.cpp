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
#include "Archer.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"
#include "Global/GlobalAttributes.h"


Archer::Archer(Player* pOwner, Fraction fraction, Map* pMap, bool isUpgraded) : Unit(pOwner, fraction, pMap, 50, 0, 5, 10), IArmed(3, 6, 4), m_berserkerRegenerationTicks(calculateRequiredTicksForGameAction(4000)) {
	if (m_fraction == Fraction::Alliance) {
		setDyingSound("human/basic_voices/dead.wav");
		setSoundBaseDirectory("human/units/elven_archer-ranger/");
	}
	else {
		setDyingSound("orc/basic_voices/dead.wav");
		setSoundBaseDirectory("orc/units/troll_axethrower-berserker/");
	}

	addAllowedUpgrade(Upgrade::ItemType::LumberMill_Arrows_1);
	addAllowedUpgrade(Upgrade::ItemType::LumberMill_Arrows_2);
	addAllowedUpgrade(Upgrade::ItemType::LumberMill_Ranger);
	addAllowedUpgrade(Upgrade::ItemType::LumberMill_RangerScouting);
	addAllowedUpgrade(Upgrade::ItemType::LumberMill_Longbow);

	if (getFraction() == Fraction::Alliance)
		addAllowedUpgrade(Upgrade::ItemType::LumberMill_Marksmanship);
	else
		addAllowedUpgrade(Upgrade::ItemType::LumberMill_TrollRegeneration);

	if (isUpgraded) {
		installUpgrade(gMapProductionItems[Upgrade::ItemType::LumberMill_Ranger]);
	}
	else {
		m_icon = g_pRessourceManager->getIcon(PUD_ICON_ARCHER, this->getFraction(), getOwner()->getColor());
		setName((getFraction() == Fraction::Alliance) ? "Elven Archer" : "Troll");
	}

	setIdleAnimation();
}


void Archer::installUpgrade(const Upgrade& u) {
	Unit::installUpgrade(u);

	if (u.getItemType() == Upgrade::ItemType::LumberMill_Ranger) {
		m_icon = g_pRessourceManager->getIcon(PUD_ICON_RANGER, this->getFraction(), getOwner()->getColor());
		setName((getFraction() == Fraction::Alliance) ? "Elven Ranger" : "Berserker");
	}
}


void Archer::process() {
	Unit::process();

	if (hasUpgrade(Upgrade::ItemType::LumberMill_TrollRegeneration)) {
		if (++m_tickCnt == m_berserkerRegenerationTicks) {
			increaseHealth(1);
			m_tickCnt = 0;
		}
	}
}
