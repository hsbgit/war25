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
#include "Foundry.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"


Foundry::Foundry(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 175, 750, 3) {
	setProperties();
}


Foundry::Foundry(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 750, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Foundry::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_FOUNDRY, this->getFraction(), getOwner()->getColor());

	setName("Foundry");
	setSoundSelected("buildings/foundry.wav");
}


std::vector<ProductionItem::ItemType> Foundry::getProductionItemList() const {
	ProductionItem::ItemType itemCannon = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::Foundry_Ship_Cannon_1) ? ProductionItem::ItemType::Foundry_Ship_Cannon_1 : ProductionItem::ItemType::Foundry_Ship_Cannon_2;
	ProductionItem::ItemType itemArmor = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::Foundry_Ship_Armor_1) ? ProductionItem::ItemType::Foundry_Ship_Armor_1 : ProductionItem::ItemType::Foundry_Ship_Armor_2;

	std::vector<ProductionItem::ItemType> vecProductionItems = { itemCannon, itemArmor };
	return vecProductionItems;
}

