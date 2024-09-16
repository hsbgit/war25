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
#include "GnomishInventor.h"
#include "Player.h"
#include "Global/Action.h"
#include "Global/TechnologyManager.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Resources/resourcemanager_wartools.h"


GnomishInventor::GnomishInventor(Peasant* pErecter, Map* pMap) : Building(pErecter, pMap, 175, 700, 3) {
	setProperties();
}


GnomishInventor::GnomishInventor(Player* pOwner, Fraction fraction, Map* pMap) : Building(pOwner, fraction, pMap, 500, 3) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


void GnomishInventor::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_GNOMISH_INVENTOR, this->getFraction(), getOwner()->getColor());

	if (getFraction() == Fraction::Alliance) {
		setName("Gnomish Inventor");
		setSoundSelected("human/buildings/gnomish_inventor.wav");
	}
	else {
		setName("Goblin Alchemist");
		setSoundSelected("orc/buildings/goblin_alchemist.wav");
	}
}


std::vector<ProductionItem::ItemType> GnomishInventor::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems;

	TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

	if (pTechManager->unitRequirementsFulfilled(typeid(GnomishFlyingMaschine)))
		vecProductionItems.push_back(ProductionItem::ItemType::GnomishFlyingMaschine);

	vecProductionItems.push_back(ProductionItem::ItemType::Dwarfes);

	return vecProductionItems;
}
