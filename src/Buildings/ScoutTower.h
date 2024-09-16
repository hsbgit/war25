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

#include "Building.h"
#include "Units/IArmed.h"


class ScoutTower : public Building, public IArmed {
public:
	enum class UpgradeLevel { ScoutTower, GuardTower, CannonTower, UpgradingToGuardTower, UpgradingToCannonTower };

	ScoutTower(Peasant* pErecter, Map* pMap);
	ScoutTower(Player* pOwner, Fraction fraction, Map* pMap, ScoutTower::UpgradeLevel state = UpgradeLevel::ScoutTower);

	// Object
	void process() override;

	inline bool isBuildingCurrentlyUpgrading() const {
		return (m_upgradeLevel == UpgradeLevel::UpgradingToGuardTower) || (m_upgradeLevel == UpgradeLevel::UpgradingToCannonTower);
	}

	// EventProcessor & BuildingEventProcessor
	void handleEvent(const Event& event) override;
	std::vector<ProductionItem::ItemType> getProductionItemList() const override;


	float getRelativeProgressOfCurrentEvent() const override {
		assert(isEventInProgress());
		return (float)(m_tickCnt) / (float)(m_requiredBuildingUpgradeTicks);
	}

	bool isEventInProgress() const override {
		return isBuildingCurrentlyUpgrading();
	}

	const Sprite& getCurrentEventIcon() const override {
		assert(isBuildingCurrentlyUpgrading());
		return m_upgradeSprite;
	}


	// IArmed
	bool canAttackAirUnits() const override {
		return (m_upgradeLevel == UpgradeLevel::GuardTower);
	}

	// Both are always zero as towers cannot be upgraded
	int getAdditionalPiercingDamage() const override {
		return 0;
	}

	int getAdditionalAttackRange() const override {
		return 0;
	}

	inline ScoutTower::UpgradeLevel getUpgradeLevel() const {
		return m_upgradeLevel;
	}

protected:
	// Building
	void onConstructionFinished() override;

private:
	Sprite m_upgradeSprite;
	UpgradeLevel m_upgradeLevel;

	int m_tickCnt = 0;
	int m_requiredBuildingUpgradeTicks = 0;

	void setScoutTowerProperties();
	void setGuardTowerProperties();
	void setCannonTowerProperties();
};
