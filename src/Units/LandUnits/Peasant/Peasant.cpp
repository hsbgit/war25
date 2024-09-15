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
#include "Peasant.h"
#include "Player.h"
#include "Buildings/Goldmine.h"
#include "Global/GlobalAttributes.h"
#include "Resources/resourcemanager_wartools.h"
#include "Global/TechnologyManager.h"
#include "EventHandling/StateMoving.h"
#include "CompositeStateMining/CompositeStateMining.h"
#include "CompositeStateChopWood/CompositeStateChopWood.h"
#include "CompositeStateConstructBuilding/CompositeStateConstructBuilding.h"

// For the peasant actions (=build building)
#include "Buildings/Church.h"
#include "Buildings/Stables.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Foundry.h"
#include "Buildings/OilRefinery.h"

#include <iostream>


// speed: 	10 7 (carrying goods)
Peasant::Peasant(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 30, 0, 4, 10), IArmed(3, 2, 1), IResourceCollector() {
	if (m_fraction == Fraction::Alliance) {
		setDyingSound("human/basic_voices/dead.wav");
		setSoundBaseDirectory("human/units/peasant/");
		setName("Peasant");
	}
	else {
		setDyingSound("orc/basic_voices/dead.wav");
		setSoundBaseDirectory("orc/basic_voices/"); // Grunt and Peon have identical sounds?
		setName("Peon");
	}

	// Specific Behavior
	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_PEASANT, this->getFraction(), getOwner()->getColor());


	m_pConstructBuilding = new CompositeStateConstructBuilding(this);
	m_pConstructBuilding->addTransition(EventID(GeneralActions::Stop), m_pIdle);


	m_pHarvestGold = new CompositeStateMining(this);
	m_pHarvestGold->addTransition(EventID(GeneralActions::Stop), m_pIdle); // Todo make me work

	// ToDo: Connect gold with wood state if goldmine is empty
	m_pHarvestWood = new CompositeStateChopWood(this);
	m_pHarvestWood->addTransition(EventID(GeneralActions::Stop), m_pIdle); // Todo make me work


	m_pIdle->addTransition(EventID(PeasantActions::HarvestGold), m_pHarvestGold);
	m_pIdle->addTransition(EventID(PeasantActions::HarvestWood), m_pHarvestWood);

	for (int action = static_cast<int>(PeasantActions::BuildBegin) + 1;
		action < static_cast<int>(PeasantActions::BuildEnd);
		action++) {

		PeasantActions actionType = static_cast<PeasantActions>(action);
		m_pIdle->addTransition(EventID(actionType), m_pConstructBuilding);
	}


	m_pMoving->addTransition(EventID(PeasantActions::HarvestGold), m_pHarvestGold);
	m_pMoving->addTransition(EventID(PeasantActions::HarvestWood), m_pHarvestWood);
	m_pMoving->addTransition(EventID(PeasantActions::BuildFarm), m_pConstructBuilding);
}


void Peasant::handleEvent(const Event& event) {
	std::cerr << "Peasant: Received Event ID: " << std::dec << (int)event.getEventID() << std::endl;
	EventID id = event.getEventID();

	bool harvestEvent = false;

	if ((GeneralActions)id == GeneralActions::Move || (PeasantActions)id == PeasantActions::Harvest) {
		Point m_targetPos_world;
		m_targetPos_world.x = event.tileX();
		m_targetPos_world.y = event.tileY();

		Tile* pTargetTile = getMap()->getTile(m_targetPos_world);

		GoldMine* pGoldMine = dynamic_cast<GoldMine*>(pTargetTile->getSeaOrLandUnit());

		if (pGoldMine) {
			std::cerr << "Peasant: Harvest Gold" << std::endl;
			Unit::handleEvent(Event(getID(), PeasantActions::HarvestGold, m_targetPos_world.x, m_targetPos_world.y));  // Logic Gold vs Wood into game.cpp?
			harvestEvent = true;
		}
		else if (pTargetTile->isWood()) {
			std::cerr << "Peasant: Harvest Lumber" << std::endl;
			Unit::handleEvent(Event(getID(), PeasantActions::HarvestWood, m_targetPos_world.x, m_targetPos_world.y));
			harvestEvent = true;
		}
	}

	if (!harvestEvent) {
		Unit::handleEvent(event);
	}
}


std::vector<Action> Peasant::getActions() const {
	std::vector<Action> vecActions = Unit::getActions();



	PlayerColor color = getOwner()->getColor();
	EventProcessorID id = EventProcessor::getID();

	/*
	PUD_ICON_HUMAN_MOVE
	PUD_ICON_COLLECT
	PUD_ICON_HUMAN_ATTACK_GROUND

	PUD_ICON_ATTACK_PEASANT

	PUD_ICON_RETURN_HUMAN_RESOURCES

	PUD_ICON_HUMAN_STAND_GROUND
	PUD_ICON_HUMAN_ATTACK_GROUND   //         katapult
	PUD_ICON_HUMAN_PATROL_ALTERNATE

	PUD_ICON_BUILD_STAGE_1
	PUD_ICON_BUILD_STAGE_2

	PUD_ICON_CANCEL
	*/


	//  SubMenu1, SubMenu2, TriggerOnly, WithWorldTile, WithTargetEventProcessor 
	//  vecActions.push_back(Action(id, (EventID)PeasantActions::Stop, g_pRessourceManager->getIcon(PUD_ICON_COLLECT, getFraction(), color)));

	vecActions.push_back(Action(id, (EventID)PeasantActions::Repair, g_pRessourceManager->getIcon(PUD_ICON_REPAIR, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::Harvest, g_pRessourceManager->getIcon(PUD_ICON_COLLECT, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::Build_Stage1, g_pRessourceManager->getIcon(PUD_ICON_BUILD_STAGE_1, getFraction(), color), Action::ActionType::SubMenu1));
	vecActions.push_back(Action(id, (EventID)PeasantActions::Build_Stage2, g_pRessourceManager->getIcon(PUD_ICON_BUILD_STAGE_2, getFraction(), color), Action::ActionType::SubMenu2));

	vecActions.push_back(Action(id, (EventID)PeasantActions::BuildTownhall, g_pRessourceManager->getIcon(PUD_ICON_TOWN_HALL, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::BuildFarm, g_pRessourceManager->getIcon(PUD_ICON_FARM, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::BuildBarracks, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_BARRACKS, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::BuildLumberMill, g_pRessourceManager->getIcon(PUD_ICON_ELVEN_LUMBER_MILL, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::BuildBlacksmith, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_BLACKSMITH, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)PeasantActions::BuildScoutTower, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SCOUT_TOWER, getFraction(), color), Action::ActionType::WithWorldTile));

	TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

	if (pTechManager->buildingRequirementsFulfilled(typeid(Church)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildChurch, g_pRessourceManager->getIcon(PUD_ICON_CHURCH, getFraction(), color), Action::ActionType::WithWorldTile));

	if (pTechManager->buildingRequirementsFulfilled(typeid(Stables)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildStables, g_pRessourceManager->getIcon(PUD_ICON_STABLES, getFraction(), color), Action::ActionType::WithWorldTile));


	if (pTechManager->buildingRequirementsFulfilled(typeid(GnomishInventor)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildGnomishInventor, g_pRessourceManager->getIcon(PUD_ICON_GNOMISH_INVENTOR, getFraction(), color), Action::ActionType::WithWorldTile));

	if (pTechManager->buildingRequirementsFulfilled(typeid(GryphonAviary)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildGryphonAviary, g_pRessourceManager->getIcon(PUD_ICON_GRYPHON_AVIARY, getFraction(), color), Action::ActionType::WithWorldTile));


	if (pTechManager->buildingRequirementsFulfilled(typeid(Shipyard)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildShipyard, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SHIPYARD, getFraction(), color), Action::ActionType::WithWorldTile));

	if (pTechManager->buildingRequirementsFulfilled(typeid(Foundry)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildFoundry, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_FOUNDRY, getFraction(), color), Action::ActionType::WithWorldTile));

	if (pTechManager->buildingRequirementsFulfilled(typeid(OilRefinery)))
		vecActions.push_back(Action(id, (EventID)PeasantActions::BuildOilRefinery, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_REFINERY, getFraction(), color), Action::ActionType::WithWorldTile));

	return vecActions;
}
