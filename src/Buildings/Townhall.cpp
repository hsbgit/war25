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
#include "Townhall.h"
#include "Player.h"
#include "Map/Map.h"
#include "Units/IArmed.h"
#include "Global/Action.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Global/TechnologyManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Resources/SoundManager.h"
#include <iostream>


Townhall::Townhall(Peasant* pErecter, Map* pMap) :
	Building(pErecter, pMap, 255, 1200, 4),
	IResourceAcceptor(),
	m_upgradeLevel(UpgradeLevel::Townhall), m_upgradeSprite(g_pRessourceManager->getIcon(PUD_ICON_KEEP)) {

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_KEEP, this->getFraction(), getOwner()->getColor());
}


Townhall::Townhall(Player* pOwner, Fraction fraction, Map* pMap, UpgradeLevel upgradeLevel) :
	Building(pOwner, fraction, pMap, 1200, 4),
	IResourceAcceptor(),
	m_upgradeLevel(upgradeLevel), m_upgradeSprite(g_pRessourceManager->getIcon(PUD_ICON_KEEP))
{
	if (m_upgradeLevel == UpgradeLevel::Keep) {
		setKeepProperties();
	}
	else if (m_upgradeLevel == UpgradeLevel::Castle) {
		setCastleProperties();
	}
	else {
		onConstructionFinished();
	}

	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
	getOwner()->increaseTotalFood(1);
}


void Townhall::onConstructionFinished() {
	getOwner()->increaseTotalFood(1);
	setTownhallProperties();
}


Townhall::~Townhall() {
	getOwner()->decreaseTotalFood(1);
}


void Townhall::process() {
	Building::process();

	if (isBuildingCurrentlyUpgrading() && (++m_tickCnt == m_requiredBuildingUpgradeTicks)) {
		if (m_upgradeLevel == UpgradeLevel::UpgradingToKeep) {
			setKeepProperties();
		}
		else if (m_upgradeLevel == UpgradeLevel::UpgradingToCastle) {
			setCastleProperties();
		}
	}
}


void Townhall::handleEvent(const Event& event) {
	EventID eventID = event.getEventID();
	ProductionItem::ItemType itemType = (ProductionItem::ItemType)eventID;
	assert(gMapProductionItems.contains(itemType));

	const ProductionItem& item = gMapProductionItems[itemType];

	if (item.getItemType() == ProductionItem::ItemType::Peasant) {
		// Basic unit production; can be handled by our generic BuildingEventProcessor
		BuildingEventProcessor::handleEvent(event);
		return;
	};

	// Upgrading stuff is handled by ourselfes
	if (!getOwner()->takeFromResources(item.costs())) {
		g_pSoundManager->playErrorWAV();
		// Generate sound/msg
		std::cerr << "Called EventProcessor::handleEvent! ERROR: Not enough resources!" << std::endl;
		return;
	}

	if (itemType == ProductionItem::ItemType::Keep) {
		m_upgradeLevel = UpgradeLevel::UpgradingToKeep;
		m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor(), true);
	}
	else if (itemType == ProductionItem::ItemType::Castle) {
		m_upgradeLevel = UpgradeLevel::UpgradingToCastle;
		m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor(), true);
	}
	else {
		throw std::logic_error("Invalid EventID for ScoutTower!");
	}

	m_upgradeSprite = g_pRessourceManager->getIcon(item.getPUDIconID(), getFraction(), getOwner()->getColor());

	m_tickCnt = 0;
	m_requiredBuildingUpgradeTicks = calculateRequiredTicksForGameAction(item.researchTime());
}


float Townhall::getRelativeProgressOfCurrentEvent() const {
	assert(isEventInProgress());

	// Weg??
	if (isBuildingCurrentlyUpgrading()) {
		return (float)m_tickCnt / (float)m_requiredBuildingUpgradeTicks;
	}

	// Peasant is produced
	return BuildingEventProcessor::getRelativeProgressOfCurrentEvent();
}


bool Townhall::isEventInProgress() const {
	if (isBuildingCurrentlyUpgrading())
		return true;

	// Peasant is produced
	return BuildingEventProcessor::isEventInProgress();
}


const Sprite& Townhall::getCurrentEventIcon() const {
	if (isBuildingCurrentlyUpgrading())
		return m_upgradeSprite;

	// Peasant is produced
	return BuildingEventProcessor::getCurrentEventIcon();
}


std::vector<ProductionItem::ItemType> Townhall::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems;

	if (isBuildingCurrentlyUpgrading()) {
		// CancelItem?
		return vecProductionItems;
	}

	vecProductionItems.push_back(ProductionItem::ItemType::Peasant);

	if (!BuildingEventProcessor::isEventInProgress()) {
		TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

		if (m_upgradeLevel == UpgradeLevel::Townhall && pTechManager->buildingRequirementsFulfilled(UpgradeLevel::Keep))
			vecProductionItems.push_back(ProductionItem::ItemType::Keep);
		else if (m_upgradeLevel == UpgradeLevel::Keep && pTechManager->buildingRequirementsFulfilled(UpgradeLevel::Castle))
			vecProductionItems.push_back(ProductionItem::ItemType::Castle);
	}

	return vecProductionItems;
}


void Townhall::setTownhallProperties() {
	m_upgradeLevel = UpgradeLevel::Townhall;

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_TOWN_HALL, this->getFraction(), getOwner()->getColor());

	setHealthPointsMax(1200);
	setSightRange(4);

	setName((getFraction() == Fraction::Alliance) ? "Townhall" : "Great Hall");
}


void Townhall::setKeepProperties() {
	m_upgradeLevel = UpgradeLevel::Keep;

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_KEEP, this->getFraction(), getOwner()->getColor());
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());

	setHealthPointsMax(1400);
	setSightRange(6);
	m_pOwner->increaseResourceProduction(Resources(10));

	setName((getFraction() == Fraction::Alliance) ? "Keep" : "Stronghold");
}


void Townhall::setCastleProperties() {
	m_upgradeLevel = UpgradeLevel::Castle;

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_CASTLE, this->getFraction(), getOwner()->getColor());
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());

	setHealthPointsMax(1600);
	setSightRange(9);
	m_pOwner->increaseResourceProduction(Resources(10));

	setName((getFraction() == Fraction::Alliance) ? "Castle" : "Fortress");
}

