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

#include "Units/UnitActions.h"
#include <cassert>


class Buff {
public:
    Buff();
    Buff(BuffType t);

    void process();

    inline void reset() {
        m_lifespan_ticks = m_originalLifespan_ticks;
    }

    inline bool isExpired() const {
        assert(m_lifespan_ticks != -1);
        return m_lifespan_ticks == 0;
    }

    inline BuffType getType() const {
        return m_type;
    }

private:
    BuffType m_type = BuffType::Uninitialized;
    int m_lifespan_ticks = -1;
    int m_originalLifespan_ticks = -1;
};