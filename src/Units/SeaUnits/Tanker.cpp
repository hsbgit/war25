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
#include "Tanker.h"
#include "Resources/resourcemanager_wartools.h"
#include "Player.h"
#include "CompositeStateOil.h"
#include "Buildings/OilRig.h"
#include "Global/Action.h"
#include "Map/Map.h"
#include "Units/Resource/ResourceRules.h"


Tanker::Tanker(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 90, 0, 4, 10) {
	// Generic behavior
	if (m_fraction == Fraction::Alliance) {
		setSoundBaseDirectory("human/ships/");
	}
	else {
		setSoundBaseDirectory("orc/ships/");
	}

	setDyingSound("ships/sinking.wav");

	// Specific behavior
	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_TANKER, this->getFraction(), getOwner()->getColor());

	setName("Oil Tanker");

	m_harvestOil = new CompositeStateOil(this);
	m_harvestOil->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_harvestOil->addTransition(EventID(PeasantActions::HarvestOil), m_harvestOil);
	m_pIdle->addTransition(EventID(PeasantActions::HarvestOil), m_harvestOil);
	m_pMoving->addTransition(EventID(PeasantActions::HarvestOil), m_harvestOil);
}


Tanker::~Tanker() {
	delete m_harvestOil;
}


void Tanker::handleEvent(const Event& event) {
	if (ResourceRules::commandBlockedWhileInsideMine(
		isPlacedOnMap(), currentResourceMine() != nullptr) ||
		!isPlacedOnMap()) {
		return;
	}
	Event normalized = event;
	const EventID id = event.getEventID();
	if ((GeneralActions)id == GeneralActions::Move ||
		(PeasantActions)id == PeasantActions::Harvest) {
		Tile* target = getMap()->getTile({event.tileX(), event.tileY()});
		if (target && dynamic_cast<OilRig*>(target->getSeaOrLandUnit()))
			normalized = Event(getID(), PeasantActions::HarvestOil,
				event.tileX(), event.tileY());
	}
	Unit::handleEvent(normalized);
}


std::vector<Action> Tanker::getActions() const {
	auto actions = Unit::getActions();
	actions.push_back(Action(getID(), EventID(PeasantActions::Harvest),
		g_pRessourceManager->getIcon(PUD_ICON_HUMAN_COLLECT_OIL,
			getFraction(), getOwner()->getColor()), Action::ActionType::WithWorldTile));
	return actions;
}
