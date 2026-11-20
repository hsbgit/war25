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
#include "Goldmine.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Resources/resourcemanager_wartools.h"
#include <algorithm>
#include <cassert>


GoldMine::GoldMine(Player* pOwner, Map* pMap, int gold) :
	Building(pOwner, Fraction::Neutral, pMap, 25500, 4), IResourceMine(pMap, Resources(gold, 0, 0), calculateRequiredTicksForGameAction(2 * 1000)) {
	// Exact hitpoints and sight are unknown    

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_GOLD_MINE);
	m_sprite = g_pRessourceManager->getNeutralBuildingSprite(this);

	setName("Goldmine");
	setSoundSelected("neutral/buildings/gold_mine.wav");
}


void GoldMine::die() {
	// Release all Peasants
	for (auto it = m_mapWorkers.begin(); it != m_mapWorkers.end(); ) {
		Peasant* pPeasant = dynamic_cast<Peasant*>(it->first);

		if (placePeasant(pPeasant, false))
			it = m_mapWorkers.erase(it);
		else
			++it;
	}

	// Never remove the mine while it still owns workers hidden from the map.
	if (!m_mapWorkers.empty())
		return;

	Object::die();
}


bool GoldMine::placePeasant(Peasant* pPeasant, bool withGold) {
	Tile* pFreeTileNextToGoldMine = Object::m_pMap->getFreeTileAroundBuilding(this, pPeasant);
	if (!pFreeTileNextToGoldMine)
		pFreeTileNextToGoldMine = Object::m_pMap->getNextFreeTile(getTile(), Object::m_pMap->sideLength());

	if (!pFreeTileNextToGoldMine)
		return false;

	assert(pFreeTileNextToGoldMine->getSeaOrLandUnit() == nullptr);

	if (withGold) {
		m_availableRes -= Resources(std::min(100, m_availableRes.gold()));
		dynamic_cast<Peasant*>(pPeasant)->setCarriedResource(ResourceType::Gold);
	}

	Object::m_pMap->placeUnit(pFreeTileNextToGoldMine, pPeasant);
	pPeasant->setCurrentResourceMine(nullptr);
	return true;
}


bool GoldMine::leave(IResourceCollector* pCollector) {
	auto it = m_mapWorkers.find(pCollector);
	if (it == m_mapWorkers.end())
		return false;
	auto* peasant = dynamic_cast<Peasant*>(pCollector);
	if (!peasant || !placePeasant(peasant, false))
		return false;
	m_mapWorkers.erase(it);
	if (m_mapWorkers.empty())
		switchImage(false);
	return true;
}


void GoldMine::process() {
	Building::process();

	for (auto it = m_mapWorkers.begin(); it != m_mapWorkers.end(); ) {
		auto& remainingTicks = it->second;

		if (remainingTicks > 0) {
			remainingTicks -= 1;
		}

		if (remainingTicks == 0) {
			Peasant* pPeasant = dynamic_cast<Peasant*>(it->first);
			const bool hasGold = m_availableRes.gold() > 0;
			bool success = placePeasant(pPeasant, hasGold);

			if (success) {
				it = m_mapWorkers.erase(it);

				continue;
			}
		}

		++it;
	}

	if (!m_mapWorkers.size())
		switchImage(false);

	if (m_availableRes.gold() <= 0)
		die();
}


bool GoldMine::enter(IResourceCollector* pPeasant) {
	if (!pPeasant || isDead() || m_availableRes.gold() <= 0 ||
		m_mapWorkers.size() >= MaxWorkers || m_mapWorkers.contains(pPeasant))
		return false;

	// Switch light on, at least one Peasant is working
	switchImage(true);

	Object::m_pMap->releaseObjectFromTile(dynamic_cast<Object*>(pPeasant));
	m_mapWorkers[pPeasant] = m_miningTicks;
	pPeasant->setCurrentResourceMine(this);
	return true;
}


void GoldMine::switchImage(bool miningActive) {
	m_sprite = g_pRessourceManager->getNeutralBuildingSprite(this, miningActive ? 1 : 0);
}
