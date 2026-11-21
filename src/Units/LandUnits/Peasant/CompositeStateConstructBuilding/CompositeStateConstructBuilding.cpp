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
#include "CompositeStateConstructBuilding.h"
#include "Buildings/Goldmine.h"
#include "Buildings/Farm.h"
#include "Buildings/Barracks.h"
#include "Buildings/LumberMill.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Church.h"
#include "Buildings/Stables.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/ScoutTower.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Foundry.h"
#include "Buildings/OilRefinery.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "EventHandling/StateMoving.h"
#include "Resources/SoundManager.h"
#include "Global/GlobalAttributes.h"
#include "Global/BuildingPlacement.h"
#include "Map/Map.h"
#include <iostream>


template<typename BuildingType>
Building* createBuilding(PeasantActions action, const Point& targetPos_world, Peasant* pErecter) {
	if (pErecter->getMap()->validateBuildingPlacement(action, targetPos_world, pErecter).allowed) {
		return new BuildingType(pErecter, pErecter->getMap());
	}
	return nullptr;
}


void CompositeStateConstructBuilding::onWorkDone() {
	std::cout << "callback onWorkDone!!!" << std::endl;

	Peasant* pErecter = dynamic_cast<Peasant*>(m_pOwner);

	if (!pErecter->getTile() || pErecter->getTile()->getPos() != m_approachPos_world) {
		std::cout << " \tpeasant has not reached the actual building tile: Not building!" << std::endl;
		return;
	}

	Building* pBuilding = nullptr;

	// Remove it to "make space": We want to start building and our Peasant is blocking the upper left tile of the desired construction site
	pErecter->getMap()->releaseObjectFromTile(pErecter);

	switch (m_buildingToConstruct) {
	case PeasantActions::BuildTownhall:
		pBuilding = createBuilding<Townhall>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildFarm:
		pBuilding = createBuilding<Farm>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildBarracks:
		pBuilding = createBuilding<Barracks>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildLumberMill:
		pBuilding = createBuilding<LumberMill>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildBlacksmith:
		pBuilding = createBuilding<Blacksmith>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildChurch:
		pBuilding = createBuilding<Church>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildStables:
		pBuilding = createBuilding<Stables>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildGnomishInventor:
		pBuilding = createBuilding<GnomishInventor>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildGryphonAviary:
		pBuilding = createBuilding<GryphonAviary>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;

	case PeasantActions::BuildScoutTower:
		pBuilding = createBuilding<ScoutTower>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;

	case PeasantActions::BuildShipyard:
		pBuilding = createBuilding<Shipyard>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildFoundry:
		pBuilding = createBuilding<Foundry>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;
	case PeasantActions::BuildOilRefinery:
		pBuilding = createBuilding<OilRefinery>(m_buildingToConstruct, m_targetPos_world, pErecter);
		break;

	default:
		std::cerr << "StateConstructBuilding::construct id not yet handled: " << std::dec << (int)m_buildingToConstruct << std::endl;
	}

	if (pBuilding) {
		g_pSoundManager->playWAV("ui/placement_success.wav");
		pErecter->getMap()->placeUnit(m_targetPos_world.y, m_targetPos_world.x, pBuilding);
	}
	else {
		g_pSoundManager->playWAV("ui/placement_error.wav");
		// Sth. is blocking the construction site. Show Peasant and switch to idle
		pErecter->getMap()->placeUnit(m_approachPos_world.y, m_approachPos_world.x, pErecter);
	}
}


CompositeStateConstructBuilding::CompositeStateConstructBuilding(Peasant* pPeasant) : State(pPeasant) {
	m_movingToConstructionSite = new StateMoving(pPeasant);

	m_movingToConstructionSite->registerCallback(std::bind(&CompositeStateConstructBuilding::onWorkDone, this));


	// Transitions
	f_isdead = [this]() {
		return m_pOwner->isDead();
	};

	m_movingToConstructionSite->addTransition(new ConditionBoolean(f_isdead), pPeasant->m_pDead);


	// When moving to the GoldMine or Townhall the Peasant can be interrupted with a new moving order
	m_movingToConstructionSite->addTransition(EventID(GeneralActions::Move), pPeasant->m_pMoving);

	m_movingToConstructionSite->setNextState(pPeasant->m_pIdle);
	m_pCurrState = m_movingToConstructionSite;
}


CompositeStateConstructBuilding::~CompositeStateConstructBuilding() {
	delete m_movingToConstructionSite;
}


void CompositeStateConstructBuilding::onEnter(const Event* pEvent) {
	assert(pEvent);
	m_pCurrState = m_movingToConstructionSite;
	m_buildingToConstruct = static_cast<PeasantActions>(pEvent->getEventID());
	m_targetPos_world.x = pEvent->tileX();
	m_targetPos_world.y = pEvent->tileY();
	Tile* approach = m_pOwner->getMap()->getBuilderApproachTile(
		m_buildingToConstruct, m_targetPos_world, m_pOwner);
	if (!approach || !m_pOwner->getMap()->validateBuildingPlacement(
		m_buildingToConstruct, m_targetPos_world).allowed) {
		g_pSoundManager->playWAV("ui/placement_error.wav");
		m_approachPos_world = {-1, -1};
		static_cast<StateMoving*>(m_movingToConstructionSite)->setTargetTile(m_pOwner->getTile()->getPos());
		return;
	}
	m_approachPos_world = approach->getPos();
	static_cast<StateMoving*>(m_movingToConstructionSite)->setTargetTile(m_approachPos_world);
}


State* CompositeStateConstructBuilding::process() {
	State* pNextState = m_pCurrState->process();
	m_pCurrState = pNextState;
	return m_pCurrState;
}
