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
#include "IMagical.h"
#include "Global/GlobalAttributes.h"


IMagical::IMagical() : m_mana(255), m_maxMana(255) {
    m_neededTicks = calculateRequiredTicksForGameAction(500);
    m_remainingTicks = m_neededTicks;
}


void IMagical::process() {
    if (m_remainingTicks-- == 0) {
        m_remainingTicks = m_neededTicks;
        m_mana = std::min(++m_mana, m_maxMana);
    }
}


bool IMagical::reduceMana(int manaToReduce) {
    if (manaToReduce > m_mana) {
        return false;
    }

    m_mana -= manaToReduce; 
    return true;
}
