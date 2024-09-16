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
#include "ScoutTower.h"
#include "Player.h"
#include "Map/Map.h"
#include "Map/MapObject.h"
#include "Global/Action.h"
#include "Units/IArmed.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Resources/SoundManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Units/Projectile.h"
#include <iostream>


ScoutTower::ScoutTower(Peasant* pErecter, Map* pMap) :
	Building(pErecter, pMap, 60, 100, 9),
	IArmed(), // Can be armed by upgrading to Guard or Cannon tower
	m_upgradeLevel(UpgradeLevel::ScoutTower), m_upgradeSprite(g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SCOUT_TOWER)) {

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SCOUT_TOWER, this->getFraction(), getOwner()->getColor());
	setHealthPointsMax(100);
	setName((getFraction() == Fraction::Alliance) ? "Scout Tower" : "Watch Tower");
	// toto: sight wrong (not 9 when in construction...)
}


ScoutTower::ScoutTower(Player* pOwner, Fraction fraction, Map* pMap, UpgradeLevel state) :
	Building(pOwner, fraction, pMap, 100, 9),
	IArmed(), // Can be armed by upgrading to Guard or Cannon tower
	m_upgradeLevel(state), m_upgradeSprite(g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SCOUT_TOWER)) {

	if (m_upgradeLevel == UpgradeLevel::GuardTower) {
		setGuardTowerProperties();
	}
	else if (m_upgradeLevel == UpgradeLevel::CannonTower) {
		setCannonTowerProperties();
	}
	else {
		setScoutTowerProperties();
	}
}


void ScoutTower::onConstructionFinished() {
	Building::onConstructionFinished();
	setScoutTowerProperties();
}


void ScoutTower::process() {
	Building::process();

	if (isUnderConstruction())
		return;


	if (isBuildingCurrentlyUpgrading() && (++m_tickCnt == m_requiredBuildingUpgradeTicks)) {
		if (m_upgradeLevel == UpgradeLevel::UpgradingToGuardTower) {
			setGuardTowerProperties();
		}
		else if (m_upgradeLevel == UpgradeLevel::UpgradingToCannonTower) {
			setCannonTowerProperties();
		}

		m_tickCnt = m_AttackCooldownTime_ticks;
	}

	// ToDo: Rework following code
	if (!isBuildingCurrentlyUpgrading() && isArmed()) {
		// Redundant Code in ScoutTower and SeaUnit -> Refactoring
		// alles in armed verschieben?
		// isAttackInProgress
		// cooldown etc?

		// IArmed::findTarget
		if (!hasActiveTarget()) {
			std::set<Object*> setEnemiesInRange = IArmed::findTargetObject(this);

			if (setEnemiesInRange.size()) {
				// Do sth. great, find weakest target, whatever
				// abstrakte methode für unterklassen, von IArmed aufgerufen

				setActiveTarget(*setEnemiesInRange.begin());
			}
		}
		else {
			// Todo: (wenn feind reichweite verlässt, angriff abbrechen. z.b. bei ballista). 
			if (m_tickCnt++ == m_AttackCooldownTime_ticks) {
				ResourceManager_wartools::MissileType type = (m_upgradeLevel == UpgradeLevel::GuardTower) ? ResourceManager_wartools::MissileType::Arrow : ResourceManager_wartools::MissileType::Cannon;

				getMap()->addMapObject(new Projectile(type, this, getMap(), m_pActiveTarget, { 16, 16 })); // 16,16 offset as we have a 2x2 building and want to start the arrow from center
				m_tickCnt = 0;
			}
		}
	}
}


void ScoutTower::handleEvent(const Event& event) {
	EventID eventID = event.getEventID();
	ProductionItem::ItemType itemType = (ProductionItem::ItemType)eventID;
	assert(gMapProductionItems.contains(itemType));

	const ProductionItem& item = gMapProductionItems[itemType];

	if (!getOwner()->takeFromResources(item.costs())) {
		g_pSoundManager->playErrorWAV();
		// Generate sound/msg
		std::cerr << "Called EventProcessor::handleEvent! ERROR: Not enough resources!" << std::endl;
		return;
	}

	if (itemType == ProductionItem::ItemType::GuardTower) {
		m_upgradeLevel = UpgradeLevel::UpgradingToGuardTower;
		m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor(), true);
	}
	else if (itemType == ProductionItem::ItemType::CannonTower) {
		m_upgradeLevel = UpgradeLevel::UpgradingToCannonTower;
		m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor(), true);
	}
	else {
		throw std::logic_error("Invalid EventID for ScoutTower!");
	}

	m_upgradeSprite = g_pRessourceManager->getIcon(item.getPUDIconID(), getFraction(), getOwner()->getColor());

	m_tickCnt = 0;
	m_requiredBuildingUpgradeTicks = calculateRequiredTicksForGameAction(item.researchTime());
}


std::vector<ProductionItem::ItemType> ScoutTower::getProductionItemList() const {
	std::vector<ProductionItem::ItemType> vecProductionItems;

	if (m_upgradeLevel == UpgradeLevel::ScoutTower && !isBuildingCurrentlyUpgrading()) {
		TechnologyManager* pTechManager = getOwner()->getTechnologyCoordinator();

		if (pTechManager->buildingRequirementsFulfilled(UpgradeLevel::GuardTower))
			vecProductionItems.push_back(ProductionItem::ItemType::GuardTower);

		if (pTechManager->buildingRequirementsFulfilled(UpgradeLevel::CannonTower))
			vecProductionItems.push_back(ProductionItem::ItemType::CannonTower);
	}
	else {
		// CancelItem?
	}

	return vecProductionItems;
}


void ScoutTower::setScoutTowerProperties() {
	m_upgradeLevel = UpgradeLevel::ScoutTower;
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SCOUT_TOWER, this->getFraction(), getOwner()->getColor());

	setHealthPointsMax(100);
	setName((getFraction() == Fraction::Alliance) ? "Scout Tower" : "Watch Tower");
}


void ScoutTower::setGuardTowerProperties() {
	m_upgradeLevel = UpgradeLevel::GuardTower;
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_GUARD_TOWER, this->getFraction(), getOwner()->getColor());

	setHealthPointsMax(130);
	setArmedProperties(4, 12, 6, calculateRequiredTicksForGameAction(2000));
	setName("Guard Tower");
}


void ScoutTower::setCannonTowerProperties() {
	m_upgradeLevel = UpgradeLevel::CannonTower;
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_CANNON_TOWER, this->getFraction(), getOwner()->getColor());

	setHealthPointsMax(160);
	setArmedProperties(50, 0, 7, calculateRequiredTicksForGameAction(2000));
	setName("Cannon Tower");
}


