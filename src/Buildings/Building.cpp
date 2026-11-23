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

	m_constructionTotalTicks = calculateRequiredTicksForGameAction(constructionTime_s * 1000);

	m_sprite = g_pRessourceManager->getBuildingConstructionSprite(1);

	int r = PseudoRandom::cosmeticRnd(1, 3);
	setDyingSound("misc/building_explosion/" + std::to_string(r) + ".wav");
}


Building::Building(Player* pOwner, Fraction fraction, Map* pMap, int healthPointsMax, int sight, int armor) : Object(pOwner, pMap, healthPointsMax, armor, sight), BuildingEventProcessor(this, fraction, pMap), m_currState(Building::BuildingState::Constructed), m_sprite(g_pRessourceManager->getUISprite("orc//cursors//red_crosshairs.png")) {
	m_fraction = fraction;

	int r = PseudoRandom::cosmeticRnd(1, 3);
	setDyingSound("misc/building_explosion/" + std::to_string(r) + ".wav");
}


Building::~Building() {
	// Normal map teardown is not destruction in the simulation. In particular,
	// changing maps must not emit a battlefield full of explosion/death sounds.
	if (!isDead()) return;
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

	std::cerr << "Building 0x" << std::hex << this << " destroyed" << std::endl;
}


void Building::process() {
	if (isDead()) {
		m_currState = BuildingState::Destroyed;
		return;
	}

	if (isUnderConstruction()) {
		if (!m_constructionSpriteInitialized) {
			m_sprite = g_pRessourceManager->getBuildingConstructionSprite(this, 1);
			m_constructionSpriteInitialized = true;
		}
		handleBuildProgress();
		return;
	}
	// A completed building may temporarily be surrounded. Keep trying until
	// its builder can safely leave instead of losing the unit inside forever.
	if (m_pErecter) releaseErecter();

	BuildingEventProcessor::doEventHandling();
	if (m_ambientAnimation)
		m_ambientAnimation->process();
	updateFireAnimation();
}


void Building::updateFireAnimation() {
	const float health = getRelativeHealth();
	const FireLevel desired = health <= 1.0f / 3.0f
		? FireLevel::Large
		: (health <= 2.0f / 3.0f ? FireLevel::Small : FireLevel::None);
	if (desired != m_fireLevel) {
		m_fireLevel = desired;
		if (m_fireLevel != FireLevel::None)
			m_fireAnimation = g_pRessourceManager->getBuildingFireAnimation(
				m_fireLevel == FireLevel::Large);
	}
	if (m_fireLevel != FireLevel::None)
		m_fireAnimation.process();
}


const Sprite* Building::fireSprite() const {
	return m_fireLevel == FireLevel::None ? nullptr : &m_fireAnimation.getCurrentFrame();
}


void Building::handleBuildProgress() {
	if (m_constructionElapsedTicks >= m_constructionTotalTicks)
		return;

	++m_constructionElapsedTicks;
	// Derive progress from total elapsed time instead of truncating a
	// ticks-per-hitpoint value. At 1x the configured seconds are therefore
	// exact even when the hitpoint count does not divide the tick count.
	m_healthPoints = 1 + static_cast<int>(
		(static_cast<long long>(m_constructionElapsedTicks) * (m_healthPointsMax - 1)) /
		m_constructionTotalTicks);

	if (m_currState == BuildingState::InConstruction1 &&
		m_healthPoints >= m_healthPointsMax / 3) {
		m_currState = BuildingState::InConstruction2;
		m_sprite = g_pRessourceManager->getBuildingConstructionSprite(this, 2);
		g_pSoundManager->playWAV(this, "misc/building_construction.wav");
	}
	if (m_currState == BuildingState::InConstruction2 &&
		m_healthPoints >= m_healthPointsMax / 3 * 2) {
		m_currState = BuildingState::InConstruction3;
		m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor(), true);
		g_pSoundManager->playWAV(this, "misc/building_construction.wav");
	}
	if (m_healthPoints >= m_healthPointsMax) {
		m_healthPoints = m_healthPointsMax;
		m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
		m_currState = BuildingState::Constructed;
		onConstructionFinished();
	}
}

bool Building::addConstructionHitPoint() {
	if (!isUnderConstruction() || m_constructionElapsedTicks >= m_constructionTotalTicks)
		return false;

	const int previousHealth = m_healthPoints;
	// Construction is time based. Advance just far enough to obtain one hit
	// point so an assisting worker follows Warcraft II's 8 HP/s repair rate.
	while (isUnderConstruction() && m_healthPoints == previousHealth)
		handleBuildProgress();
	return m_healthPoints > previousHealth;
}


void Building::onConstructionFinished() {
	assert(m_pErecter);

	if (getFraction() == Fraction::Alliance)
		g_pSoundManager->playWAV("human/basic_voices/work_complete.wav");
	else
		g_pSoundManager->playWAV("orc/basic_voices/work_complete.wav");

	releaseErecter();
}

bool Building::releaseErecter() {
	if (!m_pErecter)
		return true;
	Tile* freeTile = Object::m_pMap->getFreeTileAroundBuilding(this, m_pErecter);
	if (!freeTile)
		return false;
	Object::m_pMap->placeUnit(freeTile, m_pErecter);
	m_pErecter = nullptr;
	return true;
}

void Building::handleEvent(const Event& event) {
	if (event == BuildingActions::CancelConstruction && isUnderConstruction()) {
		if (releaseErecter())
			die();
		else
			g_pSoundManager->playErrorWAV();
		return;
	}
	BuildingEventProcessor::handleEvent(event);
}


void Building::playSoundSelected() {
	if (m_fireLevel != FireLevel::None)
		g_pSoundManager->playSelectionWAV(this, "misc/burning.wav");
	else if (isUnderConstruction())
		g_pSoundManager->playSelectionWAV(this, "misc/building_construction.wav");
	else if (m_strSoundSelected.size()) {
		g_pSoundManager->playSelectionWAV(m_strSoundSelected);
	}
}



std::vector<Action> Building::getActions() const {
	if (isUnderConstruction()) {
		return {Action(EventProcessor::getID(), static_cast<EventID>(BuildingActions::CancelConstruction),
			g_pRessourceManager->getIcon(PUD_ICON_CANCEL, getFraction(), getOwner()->getColor()),
			Action::ActionType::TriggerOnly)};
	}

	TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

	PlayerColor color = getOwner()->getColor();
	EventProcessorID id = EventProcessor::getID();


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
