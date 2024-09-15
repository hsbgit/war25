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

#include <vector>

class Map;
class Tile;

class Runes {
public:
    Runes(Map* pMap);

    void process();
    std::vector<Tile*> vecRuneTiles;

    inline bool expired() const {
        return m_lifeTime_ticks <= 0;
    }

private:
    int m_lifeTime_ticks = -1;
    Map* m_pMap = nullptr;
};
