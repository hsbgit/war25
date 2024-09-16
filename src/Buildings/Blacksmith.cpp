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
#include "Blacksmith.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Global/Action.h"
#include "Resources/resourcemanager_wartools.h"


Blacksmith::Blacksmith(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 200, 775, 3) {
	setProperties();
}


Blacksmith::Blacksmith(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 775, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Blacksmith::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_BLACKSMITH, this->getFraction(), getOwner()->getColor());
	setName("Blacksmith");
	setSoundSelected("buildings/blacksmith.wav");
}


std::vector<ProductionItem::ItemType> Blacksmith::getProductionItemList() const {
	ProductionItem::ItemType itemWeapon;

	if (getFraction() == Fraction::Alliance)
		itemWeapon = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::Blacksmith_Weapons_1_Human) ? ProductionItem::ItemType::Blacksmith_Weapons_1_Human : ProductionItem::ItemType::Blacksmith_Weapons_2_Human;
	else
		itemWeapon = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::Blacksmith_Weapons_1_Orc) ? ProductionItem::ItemType::Blacksmith_Weapons_1_Orc : ProductionItem::ItemType::Blacksmith_Weapons_2_Orc;

	ProductionItem::ItemType itemArmor = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::Blacksmith_Armor_1) ? ProductionItem::ItemType::Blacksmith_Armor_1 : ProductionItem::ItemType::Blacksmith_Armor_2;
	ProductionItem::ItemType itemBallista = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::Blacksmith_Ballista_1) ? ProductionItem::ItemType::Blacksmith_Ballista_1 : ProductionItem::ItemType::Blacksmith_Ballista_2;

	std::vector<ProductionItem::ItemType> vecProductionItems = { itemWeapon, itemArmor, itemBallista };
	return vecProductionItems;
}
