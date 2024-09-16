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


class UpgradeEffect {
public:
    UpgradeEffect(int addPiercingDamage = 0, int addArmor = 0, int addHealth = 0, int addSight = 0, int addRange = 0, bool heals = false) : m_addPiercingDamage(addPiercingDamage), m_addArmor(addArmor), m_addHealth(addHealth), m_addSight(addSight), m_addRange(addRange), m_heals(heals) {

    }

    inline int additionalPiercingDamage() const {
        return m_addPiercingDamage;
    }

    inline int additionalArmor() const {
        return m_addArmor;
    }

    inline int additionalHealth() const {
        return m_addHealth;
    }

    inline int additionalSight() const {
        return m_addSight;
    }

    inline int additionalRange() const {
        return m_addRange;
    }

    inline bool heals() const {
        return m_heals;
    }

    inline UpgradeEffect& operator+=(const UpgradeEffect& rhs) {
        m_addPiercingDamage += rhs.m_addPiercingDamage;
        m_addArmor += rhs.m_addArmor;
        m_addHealth += rhs.m_addRange;
        m_addSight += rhs.m_addSight;
        m_addRange += rhs.m_addRange;
        m_heals = rhs.m_heals;
        return *this;
    }

private:
    int m_addPiercingDamage, m_addArmor, m_addHealth, m_addSight, m_addRange;
    bool m_heals;
};
