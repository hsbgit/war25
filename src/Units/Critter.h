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

#include "Resources/resourcemanager_wartools.h" // SeasonObserver
#include "EventHandling/StateMachine.h"
#include "EventHandling/StateCondition.h"


class Critter : public Unit, public SeasonObserver {
public:
	Critter(Player* pOwner, Map* pMap);

	void process() override;

	Type getType() const override {
		return Type::Land;
	}

	const Sprite& sprite() const override;
	void doHandleDeath() override;
	bool isMoving() const { // ToDo: Can be removed if Critter is switched to a real state machine
		return true;
	}

	// SeasonObserver
	void notifySeasonChanged(Season newSeason) override;

private:
	enum class State { Idle, Moving, Dead };
	State m_currState = State::Idle;

	void stateIdle();
	void stateMoving();
	void stateDead();

	Direction direction;
	Tile* pTileToMove = nullptr;

	Sprite m_sprite;

	int m_tickCnt = 0;
	int m_requiredTicksPerMove = 0;
};
