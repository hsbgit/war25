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
#include "Runes.h"
#include "Global/GlobalAttributes.h"
#include "Map/Tile.h"
#include "Map/Map.h"
#include "Resources/resourcemanager_wartools.h"
#include "Resources/SoundManager.h"


Runes::Runes(Map* pMap) : m_pMap(pMap) {
    m_lifeTime_ticks = calculateRequiredTicksForGameAction(2 * 1000 * 60); // 2 Minutes
}


void Runes::process() {
    --m_lifeTime_ticks;

    if (expired())
        return;


    auto it = std::remove_if(vecRuneTiles.begin(), vecRuneTiles.end(),
        [this](Tile* pTile) {
            if (pTile->hasObject() && pTile->getSeaOrLandUnit()) {
                pTile->getSeaOrLandUnit()->takeDamage(AttackerStats(0, 50), true);
                g_pSoundManager->playWAV(pTile, std::string("misc") + PATH_SEPARATOR + "explosion.wav");
                m_pMap->addAnimation(pTile, g_pRessourceManager->getExplosionAnimation());
                return true;
            }
            return false;
        });

    vecRuneTiles.erase(it, vecRuneTiles.end());
}

