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
#include "Building.h"
#include "Player.h"
#include "Resources/SoundManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Global/PseudoRandom.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include <cassert>
#include <iostream>


// Note: Buildings have always 20 armor
Building::Building(Peasant* pErecter, Map* pMap, int constructionTime_s, int healthPointsMax, int sight, int armor) : Object(pErecter->getOwner(), pMap, healthPointsMax, armor, sight), BuildingEventProcessor(this, pErecter->getOwner()->getFraction(), pMap), m_currState(BuildingState::InConstruction1), m_sprite(g_pRessourceManager->getBuildingConstructionSprite(1)) {
	m_pErecter = pErecter;
	m_fraction = pErecter->getFraction();
	m_healthPoints = 1;


	g_pSoundManager->playWAV(this, "misc/building_construction.wav");

	int constructionTime_ticks = calculateRequiredTicksForGameAction(constructionTime_s * 1000);
	m_construction_ticksPerHitpoint = constructionTime_ticks / m_healthPointsMax;

	m_sprite = g_pRessourceManager->getBuildingConstructionSprite(1);

	int r = PseudoRandom::rnd(1, 3);
	setDyingSound("misc/building_explosion/" + std::to_string(r) + ".wav");
}


Building::Building(Player* pOwner, Fraction fraction, Map* pMap, int healthPointsMax, int sight, int armor) : Object(pOwner, pMap, healthPointsMax, armor, sight), BuildingEventProcessor(this, fraction, pMap), m_currState(Building::BuildingState::Constructed), m_sprite(g_pRessourceManager->getUISprite("orc//cursors//red_crosshairs.png")) {
	m_fraction = fraction;

	int r = PseudoRandom::rnd(1, 3);
	setDyingSound("misc/building_explosion/" + std::to_string(r) + ".wav");
}


Building::~Building() {
	g_pSoundManager->playWAV(getTile(), std::string("misc") + PATH_SEPARATOR + "explosion.wav");
	getMap()->addAnimation(getTile(), g_pRessourceManager->getExplosionAnimation());


	bool coastalBuilding = false;
	Point renderingOffset = { 0, 0 };


	if (!isSmallBuilding()) {
		renderingOffset.x += 16;
		renderingOffset.y += 16;

		// Coastal building?
		std::vector<Tile*>& vecTiles = getOccupiedTiles();

		for (Tile* pTile : vecTiles) {
			if (pTile->isCoast() || pTile->isSea()) {
				coastalBuilding = true;
				break;
			}
		}
	}

	Animation anim = g_pRessourceManager->getDestroyedBuildingAnimation(coastalBuilding);
	anim.setOffset(renderingOffset);
	getMap()->addAnimation(getTile(), anim);


	if (m_pErecter) {
		m_pErecter->die();
	}

	// Todo: if m_pErecter -> delete
	std::cerr << "Building 0x" << std::hex << this << " destroyed" << std::endl;
}


void Building::process() {
	if (isDead()) {
		m_currState = BuildingState::Destroyed;
		return;
	}

	if (isUnderConstruction()) {
		handleBuildProgress();
		return;
	}

	BuildingEventProcessor::doEventHandling();

	// ToDo: Make it burning when damaged
	/*
	float damageRatio = (float)getHealthPoints() / (float)getMaxHealthPoints();
	if (damageRatio > 0.6) {
		m_currState = BuildingState::HeavilyDamaged;
	}
	else if (damageRatio > 0.3) {
		m_currState = BuildingState::SlightlyDamaged;
	}    */
}


void Building::handleBuildProgress() {
	if (m_tickCounter++ != m_construction_ticksPerHitpoint) {
		return;
	}

	m_tickCounter = 1;
	m_healthPoints++;

	switch (m_currState) {
	case BuildingState::InConstruction1:
	{
		if (m_healthPoints == m_healthPointsMax / 3) {
			m_currState = BuildingState::InConstruction2;
			m_sprite = g_pRessourceManager->getBuildingConstructionSprite(2);
			g_pSoundManager->playWAV(this, "misc/building_construction.wav");
		}
	}
	break;

	case BuildingState::InConstruction2:
	{
		if (m_healthPoints == m_healthPointsMax / 3 * 2) {
			m_currState = BuildingState::InConstruction3;
			m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor(), true);
			g_pSoundManager->playWAV(this, "misc/building_construction.wav");
		}
	}

	case BuildingState::InConstruction3:
	{
		if (m_healthPoints == m_healthPointsMax) {
			m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
			m_currState = BuildingState::Constructed;
			onConstructionFinished();
		}
	}
	break;

	default:
		break;
	}
}


void Building::onConstructionFinished() {
	assert(m_pErecter);

	if (getFraction() == Fraction::Alliance)
		g_pSoundManager->playWAV("human/basic_voices/work_complete.wav");
	else
		g_pSoundManager->playWAV("orc/basic_voices/work_complete.wav");

	// Place peasant on map!
	// Todo: what if no tile is free?
	Tile* pFreeTileNextToBuilding = Object::m_pMap->getFreeTileAroundBuilding(this, m_pErecter);
	assert(pFreeTileNextToBuilding);

	if (pFreeTileNextToBuilding) {
		assert(pFreeTileNextToBuilding->getSeaOrLandUnit() == nullptr);

		Object::m_pMap->placeUnit(pFreeTileNextToBuilding, m_pErecter);
	}
}


void Building::playSoundSelected() {
	if (isUnderConstruction())
		g_pSoundManager->playWAV(this, "misc/building_construction.wav");
	else if (m_strSoundSelected.size()) {
		g_pSoundManager->playWAV(m_strSoundSelected);
	}
}


std::vector<Action> Building::getActions() const {
	if (isUnderConstruction())
		return std::vector<Action>();

	TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

	PlayerColor color = getOwner()->getColor();
	EventProcessorID id = EventProcessor::getID();


	// ToDo: if EventProcessor: EventAlready queued -> nicht anzeigen (Queue-able action?)
	// Simply use Upgrade Enum ID as EventID
	std::vector<ProductionItem::ItemType> vecProductionItemList = getProductionItemList(); // Get information from subclasses which must implement "getProductionItemList"
	std::vector<Action> vecActions;
	for (const auto& i : vecProductionItemList) {
		const ProductionItem& item = gMapProductionItems[i];
		if (!item.isResearchItem() || (item.isResearchItem() && pTechManager->isResearchPossible(item.getItemType()))) {
			vecActions.push_back(Action(id, (EventID)item.getItemType(), g_pRessourceManager->getIcon(item.getPUDIconID(getFraction()), color)));
		}
		else {
			// Item is a research item but cannot be researched -> already researched or another building is doing the research
			// Add EmptyAction
			vecActions.push_back(Action());
		}
	}

	return vecActions;
}
