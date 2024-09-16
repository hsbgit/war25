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
#include "Units/Resource/IResourceAcceptor.h"


class Townhall : public Building, public IResourceAcceptor {
public:
	enum class UpgradeLevel { Townhall, Keep, Castle, UpgradingToKeep, UpgradingToCastle };

	Townhall(Peasant* pErecter, Map* pMap);
	Townhall(Player* pOwner, Fraction fraction, Map* pMap, Townhall::UpgradeLevel upgradeLevel = UpgradeLevel::Townhall);
	~Townhall();

	// Object
	void process() override;

	// EventProcessor & BuildingEventProcessor
	void handleEvent(const Event& event) override;
	std::vector<ProductionItem::ItemType> getProductionItemList() const override;

	// The following three methods return EventProcessor-stuff if a peasant is build, when upgrading individual stuff is returned
	float getRelativeProgressOfCurrentEvent() const override;
	bool isEventInProgress() const override;
	const Sprite& getCurrentEventIcon() const override;




	// IResourceAcceptor
	bool acceptsResource(ResourceType t) const override {
		// todo: when under construction return false
		return (t == ResourceType::Gold || t == ResourceType::Wood);
	}


	// IEventHandler
	enum class Events { ProducePeasant, Upgrade, Cancel };



	inline bool isKeepOrUpgradingTo() const {
		return (getUpgradeLevel() == UpgradeLevel::UpgradingToKeep) || (getUpgradeLevel() == UpgradeLevel::Keep);
	}

	inline bool isCastleOrUpgradingTo() const {
		return (getUpgradeLevel() == UpgradeLevel::UpgradingToCastle) || (getUpgradeLevel() == UpgradeLevel::Castle);
	}

	inline Townhall::UpgradeLevel getUpgradeLevel() const {
		return m_upgradeLevel;
	}

protected:
	// Building
	void onConstructionFinished() override;

private:
	inline bool isBuildingCurrentlyUpgrading() const {
		return (m_upgradeLevel == UpgradeLevel::UpgradingToKeep || m_upgradeLevel == UpgradeLevel::UpgradingToCastle);
	}

	Sprite m_upgradeSprite;
	UpgradeLevel m_upgradeLevel;

	int m_tickCnt = 0;
	int m_requiredBuildingUpgradeTicks = 0;

	void setTownhallProperties();
	void setKeepProperties();
	void setCastleProperties();
};
