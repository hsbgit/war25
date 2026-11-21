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

#include "EventHandling/StateIdle.h"
#include <functional>

class Unit;
class StateMoving;


class CompositeStatePatrol : public State {
public:
	CompositeStatePatrol(Unit* pUnit);
	~CompositeStatePatrol() override;

	void onEnter(const Event* pEvent = nullptr) override;

	State* process() override;
	bool isMovingState() const override { return true; }

private:
	State* m_pCurrState = nullptr;

	StateMoving* m_movingToStartPoint = nullptr;
	StateMoving* m_movingToPatrolPoint = nullptr;
};
