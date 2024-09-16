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
#include "LumberMill.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Resources/resourcemanager_wartools.h"
#include "Global/Action.h"
#include <iostream>


LumberMill::LumberMill(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 150, 600, 3) {
	setProperties();
}


LumberMill::LumberMill(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 600, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void LumberMill::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_ELVEN_LUMBER_MILL, this->getFraction(), getOwner()->getColor());

	setName((getFraction() == Fraction::Alliance) ? "Lumber Mill" : "Troll Lumber Mill");
	setSoundSelected("buildings/lumbermill.wav");
}


std::vector<ProductionItem::ItemType> LumberMill::getProductionItemList() const {
	ProductionItem::ItemType itemArrows = !getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::LumberMill_Arrows_1) ? ProductionItem::ItemType::LumberMill_Arrows_1 : ProductionItem::ItemType::LumberMill_Arrows_2;

	std::vector<ProductionItem::ItemType> vecProductionItems = { itemArrows };

	if (getOwner()->getTechnologyCoordinator()->isResearched(ProductionItem::ItemType::LumberMill_Ranger)) {
		vecProductionItems.push_back(ProductionItem::ItemType::LumberMill_RangerScouting);
		vecProductionItems.push_back(ProductionItem::ItemType::LumberMill_Longbow);

		if (getFraction() == Fraction::Alliance)
			vecProductionItems.push_back(ProductionItem::ItemType::LumberMill_Marksmanship);
		else
			vecProductionItems.push_back(ProductionItem::ItemType::LumberMill_TrollRegeneration);
	}
	else {
		vecProductionItems.push_back(ProductionItem::ItemType::LumberMill_Ranger);
	}

	return vecProductionItems;
}
