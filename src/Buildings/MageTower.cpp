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
#include "MageTower.h"
#include "Player.h"
#include "Global/Action.h"
#include "Resources/resourcemanager_wartools.h"
#include "Global/TechnologyManager.h"
#include <cassert>


MageTower::MageTower(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 125, 700, 3) {
	setProperties();
}


MageTower::MageTower(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 500, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void MageTower::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_MAGE_TOWER, this->getFraction(), getOwner()->getColor());

	if (getFraction() == Fraction::Alliance) {
		setName("Mage Tower");
		setSoundSelected("human/buildings/mage_tower.wav");
	}
	else {
		setName("Temple of the Damned");
		setSoundSelected("orc/buildings/temple_of_the_damned.wav");
	}
}


std::vector<ProductionItem::ItemType> MageTower::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems = { ProductionItem::ItemType::Mage };

	if (getFraction() == Fraction::Alliance) {
		vecProductionItems.push_back(ProductionItem::ItemType::Mage_FlameShield);
		vecProductionItems.push_back(ProductionItem::ItemType::Mage_Slow);
		vecProductionItems.push_back(ProductionItem::ItemType::Mage_Invisibility);
		vecProductionItems.push_back(ProductionItem::ItemType::Mage_Polymorph);
		vecProductionItems.push_back(ProductionItem::ItemType::Mage_Blizzard);
	}
	else {
		vecProductionItems.push_back(ProductionItem::ItemType::DeathKnight_Haste);
		vecProductionItems.push_back(ProductionItem::ItemType::DeathKnight_RaiseTheDead);
		vecProductionItems.push_back(ProductionItem::ItemType::DeathKnight_Whirlwind);
		vecProductionItems.push_back(ProductionItem::ItemType::DeathKnight_UnholyArmor);
		vecProductionItems.push_back(ProductionItem::ItemType::DeathKnight_DeathAndDecay);
	}

	return vecProductionItems;
}


