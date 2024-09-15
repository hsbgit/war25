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

#include "Global/Resources.h"
#include "Global/Fractions.h"
#include "PlayerColor.h"
#include "Map/Map.h"
#include "Map/MapInfo.h"

#include <set>
#include <vector>
#include <cassert>

class Object;
class TechnologyManager;


class Player {
public:
    Player(const std::string& name, PlayerColor color, Fraction fraction, MapInfo* pMapInfo = nullptr, const Resources& res = Resources());
    ~Player();

    void process();


    inline MapInfo* getMapInfo() {
        return m_pMapInfo;
    }

    inline const std::string& getName() const {
        return m_name;
    }

    inline Fraction getFraction() const {
        return m_fraction;
    }

    inline bool isNeutral() const {
        return (m_color == PlayerColor::Neutral);
    }


    inline std::vector<Object*>& getObjects() {
        return m_vecObjects;
    }


    inline TechnologyManager* getTechnologyCoordinator() {
        return m_pTechnologyCoordinator;
    }


    /*
     * Called by Townhalls and Farms
     */
    void increaseTotalFood(int amount) {
        m_totalFood += amount;
    }

    void decreaseTotalFood(int amount) {
        m_totalFood -= amount;
        assert(m_totalFood >= 0);
    }


    /*
     * Called by Units
     */
    void increaseUsedFood() {
        ++m_usedFood;
    }

    void decreaseUsedFood() {
        --m_usedFood;
        assert(m_usedFood >= 0); // ToDo: Kill all sheep -> neutral player crash
    }


    inline bool isFoodAvailable() const {
        return (m_usedFood < m_totalFood);
    }

    inline int getAvailableFood() const {
        return m_totalFood;
    }

    inline int getUsedFood() const {
        return m_usedFood; // We can't directly return 'm_vecObjects.size()' since this vector might include dead objects which will vanish soon
    }

    inline void addToResources(const Resources& resources) {
        m_resources += resources;
    }

    inline bool takeFromResources(const Resources& resources) {
        if(m_resources >= resources) {
            m_resources -= resources;
            return true;
        }
        return false;
    }

    inline void increaseResourceProduction(const Resources& resources) {
        m_additionalResourceProduction += resources;
    }

    inline const Resources& availableResources() const {
        return m_resources;
    }

    inline const Resources& getAdditionResourceProduction() const {
        return m_additionalResourceProduction;
    }

    inline const Resources& getBasicResourceProduction() const {
        return m_basicResourceProduction;
    }

    inline PlayerColor getColor() const {
        return m_color;
    }


    void addObject(Object* pObject);
    void removeObject(Object* pObject);

    std::set<Object*> m_setSelectedObjects;


private:
    std::vector<Object*> m_vecObjects;

    std::string m_name;
    PlayerColor m_color;
public:
    // ToDo: Fix hack
    Fraction m_fraction;

private:

    int m_totalFood = 0, m_usedFood = 0;

    static const Resources m_basicResourceProduction;
    Resources m_resources, m_additionalResourceProduction;
    TechnologyManager* m_pTechnologyCoordinator = nullptr;
    
    int m_tickCnt = 0;
    int m_updateMapTicks = 0;
    MapInfo* m_pMapInfo = nullptr;
};
