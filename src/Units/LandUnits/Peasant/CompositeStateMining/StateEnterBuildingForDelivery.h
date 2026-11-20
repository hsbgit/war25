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

class Peasant;
class Tile;


class StateEnterBuildingForDelivery : public State {
public:
    StateEnterBuildingForDelivery(Peasant* pPeasant);

    void onEnter(const Event* pEvent = nullptr) override;

    State* process() override;
    void setResourceAcceptorTile(Tile* tile) { m_pResourceAcceptorTile = tile; }

private:
    int m_deliveryTicks = -1;
    Tile* m_pResourceAcceptorTile = nullptr;
};
