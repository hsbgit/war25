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
#include "TechnologyManager.h"
#include "Buildings/Building.h"
#include "Buildings/Barracks.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Church.h"
#include "Buildings/Foundry.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/LumberMill.h"
#include "Buildings/MageTower.h"
#include "Buildings/OilRig.h"
#include "Buildings/OilRefinery.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Stables.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Knight.h"
#include <cassert>


TechnologyManager::TechnologyManager(Player* pPlayer) : m_pPlayer(pPlayer) {

}


bool TechnologyManager::buildingRequirementsFulfilled(std::type_index buildingType) const {
    // switch does not support std::type_index 

    if(buildingType == typeid(Shipyard)) {
        return playerOwnsBuilding<LumberMill>();
    }

    if (buildingType == typeid(Foundry)) {
        return playerOwnsBuilding<Shipyard>();
    }

    if (buildingType == typeid(OilRefinery)) {
        return playerOwnsBuilding<Shipyard>();
    }
    

    if(buildingType == typeid(Stables)) {
        return playerHasTownHallWithUpgrade(Townhall::UpgradeLevel::Keep);
    }

    if(buildingType == typeid(GnomishInventor)) {
        return playerHasTownHallWithUpgrade(Townhall::UpgradeLevel::Keep);
    }


    if(buildingType == typeid(GryphonAviary)) {
        return playerHasTownHallWithUpgrade(Townhall::UpgradeLevel::Castle);
    }

    if(buildingType == typeid(MageTower)) {
        return playerHasTownHallWithUpgrade(Townhall::UpgradeLevel::Castle);
    }

    if(buildingType == typeid(Church)) {
        return playerHasTownHallWithUpgrade(Townhall::UpgradeLevel::Castle);
    }

    // Nothing to fulfill, return true
    return true;
}


bool TechnologyManager::unitRequirementsFulfilled(std::type_index unitType) const {
    if(unitType == typeid(Archer) || unitType == typeid(GnomishFlyingMaschine)) {
        return playerOwnsBuilding<LumberMill>();
    }

    if (unitType == typeid(Ballista)) {
        return playerOwnsBuilding<Blacksmith>();
    }

    if (unitType == typeid(Knight)) {
        return playerOwnsBuilding<Blacksmith>() && playerHasTownHallWithUpgrade(Townhall::UpgradeLevel::Keep);
    }

    if (unitType == typeid(Transport) || unitType == typeid(Battleship)) {
        return playerOwnsBuilding<Foundry>();
    }

    if (unitType == typeid(GnomishSubmarine)) {
        return playerOwnsBuilding<GnomishInventor>();
    }

    return true;
}


bool TechnologyManager::buildingRequirementsFulfilled(Townhall::UpgradeLevel desiredUpgrade) const {
    if(desiredUpgrade == Townhall::UpgradeLevel::Keep) {
        return playerOwnsBuilding<Barracks>();
    }

    if(desiredUpgrade == Townhall::UpgradeLevel::Castle) {
        return (playerOwnsBuilding<Barracks>() &&
                playerOwnsBuilding<Blacksmith>() &&
                playerOwnsBuilding<LumberMill>() &&
                playerOwnsBuilding<Stables>());
    }

    // Townhall can only be upgraded to Keep or Castle, so return false here
    return false;
}



bool TechnologyManager::buildingRequirementsFulfilled(ScoutTower::UpgradeLevel desiredUpgrade) const {
    if(desiredUpgrade == ScoutTower::UpgradeLevel::GuardTower) {
        return playerOwnsBuilding<LumberMill>();
    }

    if(desiredUpgrade == ScoutTower::UpgradeLevel::CannonTower) {
        return playerOwnsBuilding<Blacksmith>();
    }

    // ScoutTower can only be upgraded to Guard- or CannonTower, so return false here
    return false;
}


void TechnologyManager::installUpgrades(Object* pObject) {
    for(auto& upgrade : m_setResearchedTechnologies) {
        pObject->installUpgrade(gMapProductionItems[upgrade]);
    }
}


void TechnologyManager::researchFinished(ProductionItem::ItemType t) {
    assert(isResearchOngoing(t));
    m_setResearchOngoing.erase(t);
    m_setResearchedTechnologies.insert(t);

    // If it is an upgrade, install it in all existing units. A spell like "Slow" is no upgrade but a 'Buff', because it has a limited lifetime
    if (t > ProductionItem::ItemType::UPGRADES_BEGIN && t < ProductionItem::ItemType::UPGRADES_END) {
        const Upgrade& u = gMapProductionItems[t];

        // ToDo: Check who gets this upgrade?
        for (auto& pObject : m_pPlayer->getObjects()) {
            pObject->installUpgrade(u);
        }
    }


    // ToDo: Notify/update units depending on technology
    // increase oil, gold, wood income
    // ...
    // m_pPlayer->
}


bool TechnologyManager::playerHasTownHallWithUpgrade(Townhall::UpgradeLevel upgradeLevel) const {
    std::vector<Object*>& vecObjects = m_pPlayer->getObjects();

    for (auto& o : vecObjects) {
        Townhall* pTownhall = dynamic_cast<Townhall*>(o);
        if (pTownhall) {
            Townhall::UpgradeLevel currentLevel = pTownhall->getUpgradeLevel();

            // If a unit requires a Keep a Castle is also sufficient... 
            if ((upgradeLevel == Townhall::UpgradeLevel::Keep &&
                (currentLevel == Townhall::UpgradeLevel::Keep || currentLevel == Townhall::UpgradeLevel::Castle)) ||
                (currentLevel == upgradeLevel)) {
                return true;
            }
        }
    }

    return false;
}
