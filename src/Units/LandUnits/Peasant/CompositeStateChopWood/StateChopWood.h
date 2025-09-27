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

#include "EventHandling/State.h"
#include <vector>

class Tile;
class Peasant;

class StateLumberWood : public State {
public:
    StateLumberWood(Peasant* pPeasant, State* pIdle);

    // Set the move to wood state for when we need to find distant wood
    void setMoveToWoodState(State* pMoveToWood);

    void onEnter(const Event* pEvent = nullptr) override;
    void onExit() override;

    State* process() override;

    // Debug visualization support
    static void getActiveWoodSearchAreas(std::vector<std::vector<Tile*>>& searchAreas);
    std::vector<Tile*> getCurrentSearchArea() const;

private:
    Tile* identifyCloseByWood();
    void deregisterFromDebug();

    State* m_pIdle;
    State* m_pMoveToWood = nullptr;

    int m_requiredTicksForChop = -1;
    int m_ticksPerSecond = -1;

    Tile* m_pTileWithWood = nullptr;

    // Static tracking for debug visualization
    static std::vector<StateLumberWood*> s_activeInstances;
};
