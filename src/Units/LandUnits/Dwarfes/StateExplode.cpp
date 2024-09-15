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
#include "StateExplode.h"
#include "Dwarfes.h"
#include "Map/Map.h"
#include "Resources/resourcemanager_wartools.h"
#include "Resources/SoundManager.h"
#include <cassert>


StateExplode::StateExplode(Dwarfes* pDwarfes, State* pIdle) : StateMoving(pDwarfes), m_pIdle(pIdle) {
	setNextState(pDwarfes->m_pDead);
}


void StateExplode::onEnter(const Event* pEvent) {
	assert(pEvent);


	Point goldmine_tile_world = { pEvent->tileX(), pEvent->tileY() };

	// Typically pEvent will point to a tile with lumber on it as the user right clicked on such a tile which caused the lumbering event
	// However, AStar will not find a valid path as the target tile is blocked by lumber. Thus, we try to find a free tile nearby
	m_pNextReachableTile = m_pOwner->getMap()->getNextFreeTile(goldmine_tile_world);

	// We can't start AStar with a destiny tile that is blocked
	if (m_pNextReachableTile) {
		setTargetTile(m_pNextReachableTile->getPos());
	}
}


void StateExplode::onExit() {
	// Distance of two, see http://classic.battle.net/war2/units/gsdt.shtml:
	//      "Demolition Squads, Goblin Sappersand Trees"
	//      "Demolition Squad destroy about 2 rows of Trees."
	std::vector<Tile*> vecNeighbourTiles = m_pOwner->getMap()->getNeighbourTiles(m_pOwner->getTile(), 2);

	std::set<Object*> setAlreadyDamagedObjects; // We must avoid that dwarfes damage the same building more than once as a building stands on different tiles

	for (Tile* pTile : vecNeighbourTiles) {
		if (pTile->isSea())
			continue;

		pTile->destroy(); // Wood or rocks, if any

		// Includes the Transport, the only sea unit that can be destroyed if it is located/docked on a coastal tile
		Object* pUnit = pTile->getSeaOrLandUnit();

		if (pUnit) {
			if (!setAlreadyDamagedObjects.contains(pUnit)) {
				// "Dwarven Demolition Squads are fast units with a suicide attack that deals 400 Piercing Damage to all adjacent ground units and buildings when they detonate"
				// See: https://wowpedia.fandom.com/wiki/Demolition_Squad_(Warcraft_II)
				pUnit->takeDamage(AttackerStats(0, 400), true);
				setAlreadyDamagedObjects.insert(pUnit);
			}

			// ToDo: 400 damage only to direct neighbour tiles
			// ToDo: Currently crash after a second dwarves explosion?
		}
	}

	m_pOwner->die(); // Job on earth is done, dwarfes...
	m_pOwner->getMap()->addAnimation(m_pOwner->getTile(), g_pRessourceManager->getExplosionAnimation());
	g_pSoundManager->playWAV(m_pOwner->getTile(), std::string("misc") + PATH_SEPARATOR + "explosion.wav");
}


State* StateExplode::process() {
	if (!m_pNextReachableTile)
		return m_pIdle;

	return StateMoving::process();
}
