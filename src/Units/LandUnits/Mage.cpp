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
#include "Mage.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"
#include "Resources/SoundManager.h"
#include "Global/TechnologyManager.h"
#include "Magic/SpellExecutor.h"


// ToDo DeathKnight: +1 more range than the mage


Mage::Mage(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 60, 0, 9, 8), IArmed(0, 9, 2) {
	if (m_fraction == Fraction::Alliance) {
		setDyingSound("human/basic_voices/dead.wav");
		setSoundBaseDirectory("human/units/mage/");
		setName("Mage");
	}
	else {
		setDyingSound("orc/basic_voices/dead.wav");
		setSoundBaseDirectory("orc/units/death_knight/");
		setName("Dead Knight");
	}

	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_MAGE, this->getFraction(), getOwner()->getColor());
}


void Mage::process() {
	Unit::process();
	IMagical::process();
}


void Mage::handleEvent(const Event& event) {
	const EventID spell = event.getEventID();
	int manaCost = -1;
	switch (static_cast<int>(spell)) {
	case static_cast<int>(MageActions::Fireball): manaCost = 100; break;
	case static_cast<int>(BuffType::Slow): manaCost = 50; break;
	case static_cast<int>(MageActions::FlameShield): manaCost = 80; break;
	case static_cast<int>(BuffType::Invisibility): manaCost = 200; break;
	case static_cast<int>(MageActions::Polymorph): manaCost = 200; break;
	case static_cast<int>(MageActions::Blizzard): manaCost = 25; break;
	case static_cast<int>(DeathKnightActions::DeathCoil): manaCost = 100; break;
	case static_cast<int>(BuffType::Haste): manaCost = 50; break;
	case static_cast<int>(DeathKnightActions::RaiseTheDead): manaCost = 50; break;
	case static_cast<int>(DeathKnightActions::Whirlwind): manaCost = 100; break;
	case static_cast<int>(BuffType::UnholyArmor): manaCost = 200; break;
	case static_cast<int>(DeathKnightActions::DeathAndDecay): manaCost = 25; break;
	default: break;
	}
	if (manaCost >= 0) {
		if (reduceMana(manaCost)) {
			removeBuff(BuffType::Invisibility);
			gSpellExecutor.castSpell(this, spell, {event.tileX(), event.tileY()});
		}
		return;
	}
	Unit::handleEvent(event);
}



std::vector<Action> Mage::getActions() const {
	std::vector<Action> vecActions = Unit::getActions();

	PlayerColor color = getOwner()->getColor();
	EventProcessorID id = getID();

	TechnologyManager* pTC = getOwner()->getTechnologyCoordinator();


	if (getFraction() == Fraction::Alliance) {
		vecActions.push_back(Action(id, (EventID)MageActions::Fireball, g_pRessourceManager->getIcon(PUD_ICON_FIREBALL, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Mage_Slow))
			vecActions.push_back(Action(id, (EventID)BuffType::Slow, g_pRessourceManager->getIcon(PUD_ICON_SLOW, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Mage_FlameShield))
			vecActions.push_back(Action(id, (EventID)MageActions::FlameShield, g_pRessourceManager->getIcon(PUD_ICON_FLAME_SHIELD, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Mage_Invisibility))
			vecActions.push_back(Action(id, (EventID)BuffType::Invisibility, g_pRessourceManager->getIcon(PUD_ICON_INVISIBILITY, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Mage_Polymorph))
			vecActions.push_back(Action(id, (EventID)MageActions::Polymorph, g_pRessourceManager->getIcon(PUD_ICON_POLYMORPH, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::Mage_Blizzard))
			vecActions.push_back(Action(id, (EventID)MageActions::Blizzard, g_pRessourceManager->getIcon(PUD_ICON_BLIZZARD, color), Action::ActionType::WithWorldTile));
	}
	else {
		// Bug in pud.h: PUD_ICON_DEATH_AND_DECAY and PUD_ICON_DEATH_COIL have been mixed up
		vecActions.push_back(Action(id, (EventID)DeathKnightActions::DeathCoil, g_pRessourceManager->getIcon(PUD_ICON_DEATH_AND_DECAY, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::DeathKnight_Haste))
			vecActions.push_back(Action(id, (EventID)BuffType::Haste, g_pRessourceManager->getIcon(PUD_ICON_HASTE, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::DeathKnight_RaiseTheDead))
			vecActions.push_back(Action(id, (EventID)DeathKnightActions::RaiseTheDead, g_pRessourceManager->getIcon(PUD_ICON_SKELETON, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::DeathKnight_Whirlwind))
			vecActions.push_back(Action(id, (EventID)DeathKnightActions::Whirlwind, g_pRessourceManager->getIcon(PUD_ICON_WHIRLWIND, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::DeathKnight_UnholyArmor))
			vecActions.push_back(Action(id, (EventID)BuffType::UnholyArmor, g_pRessourceManager->getIcon(PUD_ICON_UNHOLY_ARMOR, color), Action::ActionType::WithWorldTile));

		if (pTC->isResearched(Upgrade::ItemType::DeathKnight_DeathAndDecay))
			vecActions.push_back(Action(id, (EventID)DeathKnightActions::DeathAndDecay, g_pRessourceManager->getIcon(PUD_ICON_DEATH_COIL, color), Action::ActionType::WithWorldTile));
	}

	return vecActions;
}
