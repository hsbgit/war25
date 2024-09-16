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
#include "Church.h"
#include "Global/GlobalAttributes.h"
#include "Global/Action.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"


Church::Church(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 175, 700, 3) {
	setProperties();
}


Church::Church(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 700, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Church::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_CHURCH, this->getFraction(), getOwner()->getColor());

	if (getFraction() == Fraction::Alliance) {
		setName("Church");
		setSoundSelected("human/buildings/church.wav");
	}
	else {
		setName("Altar of Storms");
		setSoundSelected("orc/buildings/altar_of_storms.wav");
	}
}


std::vector<ProductionItem::ItemType> Church::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems;

	ProductionItem::ItemType unitUpgrade = (getFraction() == Fraction::Alliance) ? ProductionItem::ItemType::Church_Paladin : Upgrade::ItemType::AltarOfStorms_OgreMage;
	ProductionItem::ItemType skill1 = (getFraction() == Fraction::Alliance) ? ProductionItem::ItemType::Church_Healing : Upgrade::ItemType::AltarOfStorms_Bloodlust;
	ProductionItem::ItemType skill2 = (getFraction() == Fraction::Alliance) ? ProductionItem::ItemType::Church_Exorcism : Upgrade::ItemType::AltarOfStorms_Runes;

	if (!getOwner()->getTechnologyCoordinator()->isResearched(unitUpgrade)) {
		vecProductionItems.push_back(unitUpgrade);
	}
	else {
		vecProductionItems.push_back(skill1);
		vecProductionItems.push_back(skill2);
	}

	return vecProductionItems;
}

