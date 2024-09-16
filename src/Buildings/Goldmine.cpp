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

		// Todo: what if this fails? same situation as when finishing a construction (buiding complete)
		bool success = placePeasant(pPeasant, false);
		assert(success);
		++it;
	}

	m_mapWorkers.clear();
	Object::die();
}


bool GoldMine::placePeasant(Peasant* pPeasant, bool withGold) {
	Tile* pFreeTileNextToGoldMine = Object::m_pMap->getFreeTileAroundBuilding(this, pPeasant);

	if (!pFreeTileNextToGoldMine)
		return false;

	assert(pFreeTileNextToGoldMine->getSeaOrLandUnit() == nullptr);

	if (withGold) {
		m_availableRes -= Resources(100);
		dynamic_cast<Peasant*>(pPeasant)->setCarriedResource(ResourceType::Gold);
	}

	Object::m_pMap->placeUnit(pFreeTileNextToGoldMine, pPeasant);
	return true;
}


void GoldMine::process() {
	Building::process();

	if (m_availableRes.gold() == 0)
		return;


	for (auto it = m_mapWorkers.begin(); it != m_mapWorkers.end(); ) {
		auto& remainingTicks = it->second;

		if (remainingTicks > 0) {
			remainingTicks -= 1;
		}

		if (remainingTicks == 0) {
			Peasant* pPeasant = dynamic_cast<Peasant*>(it->first);
			bool success = placePeasant(pPeasant, true);

			if (success) {
				it = m_mapWorkers.erase(it);

				if (m_availableRes.gold() == 0) {
					die();
					break;
				}
				continue;
			}
		}

		++it;
	}

	if (!m_mapWorkers.size())
		switchImage(false);
}


bool GoldMine::enter(IResourceCollector* pPeasant) {
	if (m_mapWorkers.size() > 4)
		return false;

	// Switch light on, at least one Peasant is working
	switchImage(true);

	Object::m_pMap->releaseObjectFromTile(dynamic_cast<Object*>(pPeasant));
	m_mapWorkers[pPeasant] = m_miningTicks;
	return true;
}


void GoldMine::switchImage(bool miningActive) {
	m_sprite = g_pRessourceManager->getNeutralBuildingSprite(this, miningActive ? 1 : 0);
}
