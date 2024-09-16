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
#include "Shipyard.h"
#include "Player.h"
#include "Global/Action.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"


Shipyard::Shipyard(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 200, 1100, 3) {
	setProperties();
}


Shipyard::Shipyard(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 1100, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void Shipyard::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SHIPYARD, this->getFraction(), getOwner()->getColor());
	setName("Shipyard");
	setSoundSelected("buildings/shipyard.wav");
}


std::vector<ProductionItem::ItemType> Shipyard::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems = { ProductionItem::ItemType::Tanker };

	TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

	if (pTechManager->unitRequirementsFulfilled(typeid(Transport)))
		vecProductionItems.push_back(ProductionItem::ItemType::Transport);

	vecProductionItems.push_back(ProductionItem::ItemType::Destroyer);

	if (pTechManager->unitRequirementsFulfilled(typeid(Battleship)))
		vecProductionItems.push_back(ProductionItem::ItemType::Battleship);

	if (pTechManager->unitRequirementsFulfilled(typeid(GnomishSubmarine)))
		vecProductionItems.push_back(ProductionItem::ItemType::GnomishSubmarine);

	return vecProductionItems;
}

