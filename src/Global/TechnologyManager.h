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
#pragma once

#include <set>
#include <typeinfo>
#include <typeindex>
#include <cassert>
#include "Buildings/Townhall.h"
#include "Buildings/ScoutTower.h"
#include "Player.h"
#include "ProductionItem.h"

class Object;


class TechnologyManager {
public:
    TechnologyManager(Player* pPlayer);

    void installUpgrades(Object* pObject);

    bool buildingRequirementsFulfilled(std::type_index buildingType) const;
    bool unitRequirementsFulfilled(std::type_index unitType) const;

    // Upgrade checks for Townhall and ScoutTower are performed in separate functions since
    // these buildings are the only ones that are upgradeable. However, there is no class "Keep"
    // or "Castle", there is only class Townhall. For that reason, the generic 'buildingRequirementsFulfilled' function
    // can not be used. Hence, we check for the desired upgrade state.
    bool buildingRequirementsFulfilled(Townhall::UpgradeLevel desiredUpgrade) const;
    bool buildingRequirementsFulfilled(ScoutTower::UpgradeLevel desiredUpgrade) const;

    inline void markResearchAsInProgress(ProductionItem::ItemType t) {
        assert(!isResearchOngoing(t));
        m_setResearchOngoing.insert(t);
    }

    // Todo: Who gives resources back to the player who cancelled research?
    inline void cancelResearch(ProductionItem::ItemType t) {
        assert(isResearchOngoing(t));
        m_setResearchOngoing.erase(t);
    }

    inline bool isResearchOngoing(ProductionItem::ItemType t) const {
        return (m_setResearchOngoing.find(t) != m_setResearchOngoing.end());
    }

    // ToDo: Special case weapons for orcs/humans
    inline bool isResearched(ProductionItem::ItemType t) const {
        return (m_setResearchedTechnologies.find(t) != m_setResearchedTechnologies.end());
    }

    inline bool isResearchPossible(ProductionItem::ItemType t) const {
        return (!isResearched(t) && !isResearchOngoing(t));
    }

    void researchFinished(ProductionItem::ItemType t);


private:
    template<class T>
    bool playerOwnsBuilding() const {
        std::vector<Object*>& vecObjects = m_pPlayer->getObjects();

        for(auto& o : vecObjects) {
			Building* pBuilding = dynamic_cast<T*>(o);
            if(pBuilding != nullptr && pBuilding->getBuildingState() == Building::BuildingState::Constructed) {
                return true;
            }
        }

        return false;
    }

	
    bool playerHasTownHallWithUpgrade(Townhall::UpgradeLevel upgradeLevel) const;

    std::set<ProductionItem::ItemType> m_setResearchOngoing, m_setResearchedTechnologies;
    Player* m_pPlayer = nullptr;
};
