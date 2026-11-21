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
#include "Units/Resource/IResourceMine.h"

// For the peasant actions (=build building)
#include "Buildings/Church.h"
#include "Buildings/Stables.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Foundry.h"
#include "Buildings/OilRefinery.h"
#include "Buildings/Building.h"
#include "Units/SeaUnits/Transport.h"
#include "EventHandling/EventBroker.h"

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
	m_pHarvestGold->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pHarvestGold->addTransition(EventID(GeneralActions::Move), m_pMoving);
	m_pHarvestGold->addTransition(EventID(PeasantActions::HarvestGold), m_pHarvestGold);

	m_pHarvestWood = new CompositeStateChopWood(this);
	m_pHarvestWood->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pHarvestWood->addTransition(EventID(GeneralActions::Move), m_pMoving);
	m_pHarvestWood->addTransition(EventID(PeasantActions::HarvestGold), m_pHarvestGold);
	m_pHarvestWood->addTransition(EventID(PeasantActions::HarvestWood), m_pHarvestWood);
	m_pHarvestGold->addTransition(EventID(PeasantActions::HarvestWood), m_pHarvestWood);


	m_pIdle->addTransition(EventID(PeasantActions::HarvestGold), m_pHarvestGold);
	m_pIdle->addTransition(EventID(PeasantActions::HarvestWood), m_pHarvestWood);

	for (int action = static_cast<int>(PeasantActions::BuildBegin) + 1;
		action < static_cast<int>(PeasantActions::BuildEnd);
		action++) {

		PeasantActions actionType = static_cast<PeasantActions>(action);
		m_pIdle->addTransition(EventID(actionType), m_pConstructBuilding);
		m_pMoving->addTransition(EventID(actionType), m_pConstructBuilding);
		m_pHarvestGold->addTransition(EventID(actionType), m_pConstructBuilding);
		m_pHarvestWood->addTransition(EventID(actionType), m_pConstructBuilding);
	}


	m_pMoving->addTransition(EventID(PeasantActions::HarvestGold), m_pHarvestGold);
	m_pMoving->addTransition(EventID(PeasantActions::HarvestWood), m_pHarvestWood);
}


void Peasant::handleEvent(const Event& event) {
	std::cerr << "Peasant: Received Event ID: " << std::dec << (int)event.getEventID() << std::endl;
	if (!isPlacedOnMap()) {
		IResourceMine* mine = currentResourceMine();
		if (mine) {
			if (!mine->leave(this))
				return;
		} else {
			// Delivery buildings temporarily hide the worker as well. Remember the
			// latest command and execute it as soon as the worker reappears.
			m_pendingCommand = event;
			return;
		}
	}
	EventID id = event.getEventID();
	if ((PeasantActions)id == PeasantActions::Repair) {
		beginRepair(event);
		return;
	}
	m_repairTarget = 0;
	m_repairTickCounter = 0;

	bool harvestEvent = false;
	Event normalizedEvent = event;

	if ((GeneralActions)id == GeneralActions::Move || (PeasantActions)id == PeasantActions::Harvest) {
		Point m_targetPos_world;
		m_targetPos_world.x = event.tileX();
		m_targetPos_world.y = event.tileY();

		Tile* pTargetTile = getMap()->getTile(m_targetPos_world);

		GoldMine* pGoldMine = dynamic_cast<GoldMine*>(pTargetTile->getSeaOrLandUnit());

		if (pGoldMine) {
			std::cerr << "Peasant: Harvest Gold" << std::endl;
			normalizedEvent = Event(getID(), PeasantActions::HarvestGold, m_targetPos_world.x, m_targetPos_world.y);
			harvestEvent = true;
		}
		else if (pTargetTile->isWood()) {
			std::cerr << "Peasant: Harvest Lumber" << std::endl;
			normalizedEvent = Event(getID(), PeasantActions::HarvestWood, m_targetPos_world.x, m_targetPos_world.y);
			harvestEvent = true;
		}
	}

	if (harvestEvent && currentlyCarrying() != ResourceType::Nothing) {
		// Remember the requested resource, but finish the current delivery first.
		m_pendingCommand = normalizedEvent;
		const PeasantActions returnCycle = currentlyCarrying() == ResourceType::Wood
			? PeasantActions::HarvestWood : PeasantActions::HarvestGold;
		Unit::handleEvent(Event(getID(), returnCycle,
			getTileLocation().x, getTileLocation().y));
		return;
	}
	if (!harvestEvent)
		m_pendingCommand.reset();

	Unit::handleEvent(harvestEvent ? normalizedEvent : event);
}


void Peasant::process() {
	Unit::process();
	processRepair();
	if (m_pendingCommand && currentlyCarrying() == ResourceType::Nothing && isPlacedOnMap()) {
		Event pending = *m_pendingCommand;
		m_pendingCommand.reset();
		handleEvent(pending);
	}
}

bool Peasant::isNextTo(const Object* target) const {
	if (!target || !isPlacedOnMap() || !target->isPlacedOnMap()) return false;
	const Point own = getTileLocation();
	for (Tile* tile : target->getOccupiedTiles()) {
		if (std::max(std::abs(own.x - tile->getX()),
			std::abs(own.y - tile->getY())) <= 1)
			return true;
	}
	return false;
}

void Peasant::beginRepair(const Event& event) {
	Object* target = getMap()->getObjectOnTile(event.tileX(), event.tileY());
	auto* processor = dynamic_cast<EventProcessor*>(target);
	const bool repairable = target && target != this && !target->isDead() &&
		target->getOwner() == getOwner() &&
		(dynamic_cast<Building*>(target) || dynamic_cast<Transport*>(target));
	if (!repairable || !processor || target->getHealthPoints() >= target->getMaxHealthPoints()) {
		m_repairTarget = 0;
		return;
	}

	m_pendingCommand.reset();
	m_repairTarget = processor->getID();
	m_repairTickCounter = 0;
	if (isNextTo(target)) {
		Unit::handleEvent(Event(getID(), GeneralActions::Stop));
		return;
	}
	if (Tile* approach = getMap()->getFreeTileAroundBuilding(target, this))
		Unit::handleEvent(Event(getID(), GeneralActions::Move,
			approach->getX(), approach->getY()));
	else
		m_repairTarget = 0;
}

void Peasant::processRepair() {
	if (!m_repairTarget || isDead() || !isPlacedOnMap()) return;
	auto* target = dynamic_cast<Object*>(g_pEventBroker.getEventHandler(m_repairTarget));
	if (!target || target->isDead() || target->getOwner() != getOwner() ||
		(!dynamic_cast<Building*>(target) && !dynamic_cast<Transport*>(target))) {
		m_repairTarget = 0;
		return;
	}
	if (!isNextTo(target)) {
		// A docked transport may have moved after the order was given.
		if (!isMoving()) beginRepair(Event(getID(), PeasantActions::Repair,
			target->getTileLocation().x, target->getTileLocation().y));
		return;
	}
	if (isMoving()) Unit::handleEvent(Event(getID(), GeneralActions::Stop));

	int interval = calculateRequiredTicksForGameAction(125); // 8 HP/s
	if (hasBuff(BuffType::Haste)) interval = std::max(1, interval / 2);
	if (hasBuff(BuffType::Slow)) interval *= 2;
	if (++m_repairTickCounter < interval) return;
	m_repairTickCounter = 0;

	if (target->getHealthPoints() >= target->getMaxHealthPoints()) {
		m_repairTarget = 0;
		return;
	}
	if (m_repairHitPointsSincePayment == 3 &&
		!getOwner()->takeFromResources(Resources(1, 1)))
		return;

	bool repaired = false;
	if (auto* building = dynamic_cast<Building*>(target); building && building->isUnderConstruction())
		repaired = building->addConstructionHitPoint();
	else {
		target->increaseHealth(1);
		repaired = true;
	}
	if (repaired) {
		m_repairHitPointsSincePayment = (m_repairHitPointsSincePayment + 1) % 4;
		if (target->getHealthPoints() >= target->getMaxHealthPoints())
			m_repairTarget = 0;
	}
}

void Peasant::onAttackedBy(Object* attacker) {
	if (!attacker || !isIdle() || !isPlacedOnMap() || !attacker->isPlacedOnMap())
		return;
	const Point own = getTileLocation();
	const Point threat = attacker->getTileLocation();
	const int dx = (own.x > threat.x) - (own.x < threat.x);
	const int dy = (own.y > threat.y) - (own.y < threat.y);
	if (dx == 0 && dy == 0) return;
	const int edge = getMap()->sideLength() - 1;
	Point destination{std::clamp(own.x + dx * 5, 0, edge),
		std::clamp(own.y + dy * 5, 0, edge)};
	// Search close to the point opposite the attacker if the ideal tile is
	// terrain-blocked. This keeps fleeing deterministic and avoids standing
	// still at map edges.
	for (int radius = 0; radius <= 4; ++radius) {
		for (int y = destination.y - radius; y <= destination.y + radius; ++y) {
			for (int x = destination.x - radius; x <= destination.x + radius; ++x) {
				if (x < 0 || y < 0 || x > edge || y > edge ||
					!getMap()->canBePlacedAtPosition(this, x, y))
					continue;
				handleEvent(Event(getID(), GeneralActions::Move, x, y));
				return;
			}
		}
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
