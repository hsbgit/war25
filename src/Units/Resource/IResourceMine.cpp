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
#include "IResourceMine.h"
#include "IResourceCollector.h"
#include "Map/Map.h"


IResourceMine::IResourceMine(Map* pMap, const Resources& availableRes, int miningTicks) : m_miningTicks(miningTicks), m_availableRes(availableRes), m_pMap(pMap) {

}


bool IResourceMine::enter(IResourceCollector* pPeasant) {
	if (m_mapWorkers.size() > 4)
		return false;

	// Switch light on / show ship under oil rig, at least one Peasant/ship is working
	switchImage(true);

	m_pMap->releaseObjectFromTile(dynamic_cast<Object*>(pPeasant));
	m_mapWorkers[pPeasant] = m_miningTicks;
	return true;
}
