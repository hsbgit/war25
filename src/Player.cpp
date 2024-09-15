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
#include "Player.h"
#include "Global/DebugManager.h"
#include "Global/Object.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include <algorithm>
#include <cassert>


const Resources Player::m_basicResourceProduction(100, 100, 100);


Player::Player(const std::string& name, PlayerColor color,  Fraction fraction, MapInfo* pMapInfo, const Resources& res) : m_name(name), m_color(color), m_fraction(fraction), m_resources(res), m_pTechnologyCoordinator(new TechnologyManager(this)), m_pMapInfo(pMapInfo) {
    m_updateMapTicks = calculateRequiredTicksForGameAction(500);
}


Player::~Player() {
    m_setSelectedObjects.clear();

    for(auto& pObj : m_vecObjects) {
        delete pObj;
    }

    delete m_pMapInfo;
    delete m_pTechnologyCoordinator;
}


void Player::process() {
    /*
	 * Process all remaining living objects.
	 * Do not use C++11 auto syntax here: Amount of objects might change
	 * since new (produced) units may be added to m_vecObjects
	 */
	for (std::size_t i = 0; i < m_vecObjects.size(); ++i) {
		Object* pObject = m_vecObjects[i];
		pObject->process();
    }
    
    // Updating visibility costs CPU time...
    // Further restriction: Only for the player playing! -> exploration map, all other additionally visibility map
    if (++m_tickCnt == m_updateMapTicks) {
        m_pMapInfo->updateMapVisibility(m_vecObjects);
        m_tickCnt = 0;
    }    
}


void Player::addObject(Object* pObject) {
    assert(pObject);
    assert(std::find(m_vecObjects.begin(), m_vecObjects.end(), pObject) == m_vecObjects.end()); // Do not add the same object twice

    m_vecObjects.push_back(pObject);
}


void Player::removeObject(Object* pObject) {
    assert(pObject);

    auto it = std::find(m_vecObjects.begin(), m_vecObjects.end(), pObject);
    assert(it != m_vecObjects.end()); // Object must have been added to player

    m_vecObjects.erase(it);
    
    m_setSelectedObjects.erase(pObject);
}
