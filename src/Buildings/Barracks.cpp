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
#include "Barracks.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Knight.h"
#include "Map/Map.h"
#include "Player.h"
#include "Global/Action.h"
#include "Global/GlobalAttributes.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"



Barracks::Barracks(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 200, 800, 3) {
	setProperties();
}


Barracks::Barracks(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 800, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Barracks::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_BARRACKS, this->getFraction(), getOwner()->getColor());
	setName("Barracks");
}


std::vector<ProductionItem::ItemType> Barracks::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems = { ProductionItem::ItemType::Footman };

	TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

	if (pTechManager->unitRequirementsFulfilled(typeid(Archer)))
		vecProductionItems.push_back(ProductionItem::ItemType::Archer);

	if (pTechManager->unitRequirementsFulfilled(typeid(Ballista)))
		vecProductionItems.push_back(ProductionItem::ItemType::Ballista);

	if (pTechManager->unitRequirementsFulfilled(typeid(Knight))) {
		if (pTechManager->isResearched(ProductionItem::ItemType::Church_Paladin)) {
			vecProductionItems.push_back(ProductionItem::ItemType::Church_Paladin);
		}
		else {
			vecProductionItems.push_back(ProductionItem::ItemType::Knight);
		}
	}

	return vecProductionItems;
}
