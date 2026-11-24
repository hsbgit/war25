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
#include "OilRig.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Resources/resourcemanager_wartools.h"
#include "Units/SeaUnits/Tanker.h"
#include "Map/Map.h"
#include <algorithm>


OilRig::OilRig(Peasant* pErecter, Map* pMap) :
	Building(pErecter, pMap, 175, 650, 3),
	IResourceMine(pMap, Resources(0, 0, 50000), calculateRequiredTicksForGameAction(2 * 1000)) {
	setProperties();
}


OilRig::OilRig(Player* pOwner, Fraction fraction, Map* pMap, int oil) :
	Building(pOwner, fraction, pMap, 650, 3),
	IResourceMine(pMap, Resources(0, 0, oil), calculateRequiredTicksForGameAction(2 * 1000)) {
	setProperties();
	m_sprite = g_pRessourceManager->getBuildingSprite(this, this->getFraction(), getOwner()->getColor());
}


bool OilRig::enter(IResourceCollector* pCollector) {
	auto* tanker = dynamic_cast<Tanker*>(pCollector);
	if (!tanker || tanker->getOwner() != getOwner() || isDead() ||
		m_availableRes.oil() <= 0 || m_mapWorkers.size() >= MaxWorkers ||
		m_mapWorkers.contains(pCollector))
		return false;
	return IResourceMine::enter(pCollector);
}


bool OilRig::placeTanker(IResourceCollector* pCollector, bool withOil) {
	auto* tanker = dynamic_cast<Tanker*>(pCollector);
	Tile* freeTile = getMap()->getFreeTileAroundBuilding(this, tanker);
	if (!freeTile)
		freeTile = getMap()->getNextFreeTile(getTile(), getMap()->sideLength(),
			Object::Type::Sea);
	if (!freeTile)
		return false;
	if (withOil) {
		const int amount = std::min(100, m_availableRes.oil());
		m_availableRes -= Resources(0, 0, amount);
		pCollector->setCarriedResource(ResourceType::Oil);
	}
	getMap()->placeUnit(freeTile, tanker);
	pCollector->setCurrentResourceMine(nullptr);
	return true;
}


bool OilRig::leave(IResourceCollector* pCollector) {
	auto it = m_mapWorkers.find(pCollector);
	if (it == m_mapWorkers.end() || !placeTanker(pCollector, false))
		return false;
	m_mapWorkers.erase(it);
	if (m_mapWorkers.empty())
		switchImage(false);
	return true;
}


void OilRig::process() {
	Building::process();
	for (auto it = m_mapWorkers.begin(); it != m_mapWorkers.end();) {
		if (it->second > 0)
			--it->second;
		if (it->second == 0 && placeTanker(it->first, m_availableRes.oil() > 0)) {
			it = m_mapWorkers.erase(it);
			continue;
		}
		++it;
	}
	if (m_mapWorkers.empty())
		switchImage(false);
}


void OilRig::die() {
	for (auto it = m_mapWorkers.begin(); it != m_mapWorkers.end();) {
		if (placeTanker(it->first, false))
			it = m_mapWorkers.erase(it);
		else
			++it;
	}
	if (m_mapWorkers.empty())
		Object::die();
}


void OilRig::switchImage(bool miningActive) {
	// The platform sheet reserves an additional working frame. Until the
	// building sprite API exposes it, keep the correct faction/season platform.
	m_sprite = g_pRessourceManager->getBuildingSprite(this, getFraction(), getOwner()->getColor());
}


void OilRig::setProperties() {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_OIL_WELL, this->getFraction(), getOwner()->getColor()); //PUD_ICON_OIL_PATCH

	setName("Oil Platform");
	setSoundSelected("buildings/oil_platform.wav");
}
