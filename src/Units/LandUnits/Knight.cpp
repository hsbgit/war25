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
#include "Knight.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"
#include "Global/TechnologyManager.h"
#include "Magic/SpellExecutor.h"


Knight::Knight(Player* pOwner, Fraction fraction, Map* pMap, bool isUpgraded) : Unit(pOwner, fraction, pMap, 90, 4, 4, 13), IArmed(8, 4, 1) {
	if (m_fraction == Fraction::Alliance) {
		setDyingSound("human/basic_voices/dead.wav");
	}
	else {
		setDyingSound("orc/basic_voices/dead.wav");
	}

	// Blacksmith upgrades
	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Armor_1);
	addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Armor_2);

	if (getFraction() == Fraction::Alliance) {
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_1_Human);
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_2_Human);

		// Church upgrades
		addAllowedUpgrade(Upgrade::ItemType::Church_Paladin);
		addAllowedUpgrade(Upgrade::ItemType::Church_Healing);
		addAllowedUpgrade(Upgrade::ItemType::Church_Exorcism);
	}
	else {
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_1_Orc);
		addAllowedUpgrade(Upgrade::ItemType::Blacksmith_Weapons_2_Orc);

		// Altar of Storms upgrades
		addAllowedUpgrade(Upgrade::ItemType::AltarOfStorms_OgreMage);
		addAllowedUpgrade(Upgrade::ItemType::AltarOfStorms_Bloodlust);
		addAllowedUpgrade(Upgrade::ItemType::AltarOfStorms_Runes);
	}


	if (isUpgraded) {
		Upgrade::ItemType unitUpgrade = (getFraction() == Fraction::Alliance) ? Upgrade::ItemType::Church_Paladin : Upgrade::ItemType::AltarOfStorms_OgreMage;
		installUpgrade(unitUpgrade);
	}
	else {
		m_icon = g_pRessourceManager->getIcon(PUD_ICON_KNIGHT, this->getFraction(), getOwner()->getColor());

		if (fraction == Fraction::Alliance) {
			setName("Knight");
			setSoundBaseDirectory("human/units/knight/");
		}
		else {
			setName("Ogre");
			setSoundBaseDirectory("orc/units/ogre/");
		}
	}

	setIdleAnimation();
}


void Knight::process() {
	Unit::process();
	IMagical::process();
}


void Knight::installUpgrade(const Upgrade& u) {
	Unit::installUpgrade(u);

	if (getFraction() == Fraction::Alliance && u.getItemType() == Upgrade::ItemType::Church_Paladin) {
		m_icon = g_pRessourceManager->getIcon(PUD_ICON_PALADIN, this->getFraction(), getOwner()->getColor());
		setName("Paladin");
		setSoundBaseDirectory("human/units/paladin/");
	}
	else if (getFraction() == Fraction::Orc && u.getItemType() == Upgrade::ItemType::AltarOfStorms_OgreMage) {
		m_icon = g_pRessourceManager->getIcon(PUD_ICON_PALADIN, this->getFraction(), getOwner()->getColor());
		setName("Ogre Mage");
		setSoundBaseDirectory("orc/units/ogre-mage/");
	}
}


std::vector<Action> Knight::getActions() const {
	std::vector<Action> vecActions = Unit::getActions();

	PlayerColor color = getOwner()->getColor();
	TechnologyManager* pTC = getOwner()->getTechnologyCoordinator();

	EventProcessorID id = gSpellExecutor.getID();

	if (hasUpgrade(Upgrade::ItemType::Church_Paladin)) {
		vecActions.push_back(Action(id, (EventID)PaladinActions::HolyVision, g_pRessourceManager->getIcon(PUD_ICON_HOLY_VISION, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Church_Healing))
			vecActions.push_back(Action(id, (EventID)PaladinActions::Heal, g_pRessourceManager->getIcon(PUD_ICON_HEALING, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Church_Exorcism))
			vecActions.push_back(Action(id, (EventID)PaladinActions::Excorcism, g_pRessourceManager->getIcon(PUD_ICON_EXORCISM, color), Action::ActionType::WithWorldTile));
	}


	if (hasUpgrade(Upgrade::ItemType::AltarOfStorms_OgreMage)) {
		vecActions.push_back(Action(id, (EventID)OgreActions::EyeOfKillrogg, g_pRessourceManager->getIcon(PUD_ICON_EYE_OF_KILROGG, color)));

		if (pTC->isResearched(Upgrade::ItemType::AltarOfStorms_Bloodlust))
			vecActions.push_back(Action(id, (EventID)BuffType::Bloodlust, g_pRessourceManager->getIcon(PUD_ICON_BLOODLUST, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::AltarOfStorms_Runes))
			vecActions.push_back(Action(gSpellExecutor.getID(), (EventID)OgreActions::Runes, g_pRessourceManager->getIcon(PUD_ICON_RUNES, color), Action::ActionType::WithWorldTile));
	}

	return vecActions;
}
