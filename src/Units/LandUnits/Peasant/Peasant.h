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

#include "Units/Unit.h"
#include "Units/IArmed.h"
#include "Units/Resource/IResourceCollector.h"
#include <optional>


class CompositeStateMining;
class CompositeStateChopWood;
class CompositeStateConstructBuilding;


class Peasant : public Unit, public IArmed, public IResourceCollector {
public:
	Peasant(Player* pOwner, Fraction fraction, Map* pMap);

	std::vector<Action> getActions() const override;

	Type getType() const override {
		return Type::Land;
	}

	// IArmed
	int getAdditionalPiercingDamage() const override {
		return m_upgradeStats.additionalPiercingDamage();
	}

	int getAdditionalAttackRange() const override {
		return m_upgradeStats.additionalRange();
	}

	// EventProcessor
	void handleEvent(const Event& event) override;
	void process() override;
	void onAttackedBy(Object* attacker) override;
	bool queueNearbyWoodHarvestIfGoldUnavailable(bool mineExists,
		bool mineAlive, int remainingGold, int searchDistance = 8);

	// IResourceCollector
	bool canCollectResource(ResourceType t) const override {
		return (t == ResourceType::Gold || t == ResourceType::Wood);
	}

private:
	void beginRepair(const Event& event);
	void processRepair();
	bool isNextTo(const Object* target) const;

	CompositeStateMining* m_pHarvestGold = nullptr;
	CompositeStateChopWood* m_pHarvestWood = nullptr;
	CompositeStateConstructBuilding* m_pConstructBuilding = nullptr;
	std::optional<Event> m_pendingCommand;
	EventProcessorID m_repairTarget = 0;
	int m_repairTickCounter = 0;
	int m_repairHitPointsSincePayment = 0;
};
