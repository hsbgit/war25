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

#include "EventHandling/StateMoving.h"

class Peasant;


class StateReturningResource : public StateMoving {

public:
    // Todo: more generalization -> oil tanker?
    StateReturningResource(Peasant* pPeasant, State* pIdle);

    void onEnter(const Event* pEvent = nullptr) override ;

    State* process() override;

private:
    State* m_pIdle = nullptr;

    // Never store a pointer to an object. A pointer to a tile is always valid!
    Tile* m_pTileResourceAcceptor = nullptr;
};


