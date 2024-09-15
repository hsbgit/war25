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
#include "Dwarfes.h"
#include "Player.h"
#include "StateExplode.h"
#include "Resources/resourcemanager_wartools.h"


Dwarfes::Dwarfes(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 40, 0, 4, 11), IArmed(4, 4, 1) {
	if (m_fraction == Fraction::Alliance) {
		setSoundBaseDirectory("human/units/dwarven_demolition_squad/");
		setName("Dwarven Demolition Squad");
	}
	else {
		setSoundBaseDirectory("orc/units/goblin_sappers/");
		setName("Goblin Sappers");
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

	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_DWARVES, this->getFraction(), getOwner()->getColor());


	m_pStateExplode = new StateExplode(this, m_pIdle);
	m_pStateExplode->addTransition(EventID(GeneralActions::Move), m_pMoving);
	m_pStateExplode->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pIdle->addTransition(EventID(DwarfesActions::Explode), m_pStateExplode);
}


Dwarfes::~Dwarfes() {
	delete m_pStateExplode;
}


std::vector<Action> Dwarfes::getActions() const {
	std::vector<Action> vecActions = Unit::getActions();

	PlayerColor color = getOwner()->getColor();
	EventProcessorID id = EventProcessor::getID();

	vecActions.push_back(Action(id, (EventID)DwarfesActions::Explode, g_pRessourceManager->getIcon(PUD_ICON_EXPLOSIVE, getFraction(), color), Action::ActionType::WithWorldTile));
	return vecActions;
}
