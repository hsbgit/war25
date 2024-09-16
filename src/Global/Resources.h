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

class Resources {
public:
    enum ResourceType {Gold, Lumber, Oil};

    Resources(int gold = 0, int lumber = 0, int oil = 0) : m_gold(gold), m_lumber(lumber), m_oil(oil) {

    }

    inline bool operator >(const Resources& e) const {
        return ((m_gold > e.m_gold) && (m_lumber > e.m_lumber) && (m_oil > e.m_oil));
    }

    inline bool operator >=(const Resources& e) const {
        return ((m_gold >= e.m_gold) && (m_lumber >= e.m_lumber) && (m_oil >= e.m_oil));
    }

    inline Resources& operator+=(const Resources& rhs){
        this->m_gold += rhs.m_gold;
        this->m_lumber += rhs.m_lumber;
        this->m_oil += rhs.m_oil;
        return *this;
    }

    inline Resources& operator-=(const Resources& rhs){
        this->m_gold -= rhs.m_gold;
        this->m_lumber -= rhs.m_lumber;
        this->m_oil -= rhs.m_oil;
        return *this;
    }

    inline int gold() const {
        return m_gold;
    }

    inline int lumber() const {
        return m_lumber;
    }

    inline int oil() const {
        return m_oil;
    }

private:
    int m_gold = 0, m_lumber = 0, m_oil = 0;
};

typedef Resources ProductionCosts;
