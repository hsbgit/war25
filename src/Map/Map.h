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


// ----------------------------------------------------------------------------
// !!! ToDo: This file needs a big refactoring !!!
// ----------------------------------------------------------------------------


#pragma once

#include <set>
#include <map>
#include <cassert>
#include <string>
#include <vector>
#include <typeindex>
#include <typeinfo>

#include "Tile.h"
#include "Resources/resourcemanager_wartools.h"
#include "PlayerColor.h"
#include "Global/Object.h"
#include <pud.h>

class Object;
class MapObject;
class Unit;
class Player;
class Building;
class ResourceManager;
class IRenderer;
class Viewport;
class ResourceManager_wartools;
class GoldMine;



class Map : public SeasonObserver {
public:
    Map(const std::string& filename, ResourceManager_wartools* pResManagerWartools);
    ~Map();

    const Point& getHumanPlayerStartPosition() const {
        return m_humanPlayerStartPosition;
    }
    Point m_humanPlayerStartPosition = { 0, 0 };

    Tile* getNextFreeTile(const Point& world_pos, int maxDistance = 10);
    Tile* getNextFreeTile(Tile* pTile, int maxDistance = 10);

    std::vector<Tile*> getNeighbourTiles(Tile* pTile, int distance = 1);
    Tile* getNeighbourTileWithWood(Tile* pTile);

 //   Townhall* isNextToTownhall(Tile* pTile, int distance = 1);
 //   GoldMine* isNextToGoldMine(Tile* pTile, int distance = 1);

    template<typename BuildingType>
    BuildingType* isNextToBuilding(Tile* pTile, int distance = 1);


    // SeasonObserver
    void notifySeasonChanged(Season newSeason) override;

    void createMiniMap();
    void renderMiniMapOverlay(Viewport* pViewPort);
    inline Texture* getMiniMapWithOverlay() const {
        assert(m_pTextureMiniMapWithOverlay);
        return m_pTextureMiniMapWithOverlay;
    }


    template<class T>
    inline std::vector<Tile*>& operator[] (T index) {
        return m_arrTileMap[index];
    }

    inline Tile* getTile(const Point& tile_world) {
        if (tile_world.y < m_arrTileMap.size() && tile_world.x < m_arrTileMap[tile_world.y].size()) {
            return m_arrTileMap[tile_world.y][tile_world.x];
        }

        return 0; // May happen if a Ogre Mage Rune is cast at the border of the map as it results in a + shape
    }

    inline int sideLength() const {
        return static_cast<int>(m_arrTileMap.size());
    }

    void removeObjects();
  

    std::set<Object *> getObjects(const Point& tileUL_world, const Point& tileLR_world);
    Object* getObjectOnTile(int tile_x_world, int tile_y_world);
    Object* getObjectOnTile(const Point& tile_world);
    void getVisibleTiles(Object *pObj, int sightRange, std::vector<Tile*>& vecVisibleTiles) const;
    std::set<Object*> getVisibleObjectsWithinRange(Object *pObj, int range, bool includeAirUnits = false, bool includeDeadUnits = false) const ;



    void addMapObject(MapObject* pObj) {
        m_vecMapObjects.push_back(pObj);
    }



    void addAnimation(Tile* pTile, const Animation& a) {
        m_mapAnimations[pTile] = a; // ToDo: Several animations could take place on the same tile...
    }
    std::unordered_map<Tile*, Animation> m_mapAnimations;


    /*
     * Added objects which exist only for a pre defined lifespan and which are not controlled by any fraction.
     * Examples: Arrows (or projectiles in any form), Whirlwind, Blizzards...
     */
    std::vector<MapObject*> m_vecMapObjects;


    bool isSpaceAvailableForUnit(int tile_y, int tile_x, int unitTileSize, Object::Type objType);


    Tile* getFreeTileAroundBuilding(Object *pBuilding, const Object *pObject);    
    Tile* getFreeTileAroundBuilding(Object* pBuilding, int unit_tilesize, Object::Type landSeaAir);


    void placeUnit(Tile *pTile, Object* pObject);
    void placeUnit(int tile_y, int tile_x, Object* pObject);
    bool canBePlacedAtPosition(Object* pObject, int tile_x, int tile_y, std::unordered_map<Tile*, bool>* mapState = nullptr);
    void releaseObjectFromTile(Object* pObject);


    void processMapObjects();
    void processPlayers();
    void draw(Renderer *pRenderer, Viewport* pViewPort);


    inline Player* getNeutralPlayer() const {
        return m_mapPlayer.at(PlayerColor::Neutral);
    }

    inline Player* humanControlledPlayer() {
        // ToDo: Den echten Menschen identifizieren        
        return (m_mapPlayer.begin())->second;

        //assert(m_mapPlayer[PlayerColor::Red]);
        //return m_mapPlayer[PlayerColor::Red];
    }

    std::map<PlayerColor, Player*> m_mapPlayer;

private:
    Texture* m_pTextureMiniMap = nullptr, * m_pTextureMiniMapWithOverlay = nullptr;

    void identifyNeighbourTiles();
    void addPlayersToMap(Pud* pPud);
    void addUnitsToMap(Pud* pPud);

    std::vector<std::vector<Tile*> > m_arrTileMap;

    ResourceManager_wartools* m_pResManagerWartools;
};


template<typename BuildingType>
BuildingType* Map::isNextToBuilding(Tile* pTile, int distance) {
    // Startkoordinaten des gegebenen Tiles
    int startY = pTile->getY();
    int startX = pTile->getX();

    // Durchlaufe alle Tiles im Bereich von "distance" um pTile herum
    for (int dy = -distance; dy <= distance; ++dy) {
        for (int dx = -distance; dx <= distance; ++dx) {
            // Berechne die Koordinaten des aktuellen Nachbartiles
            int y = startY + dy;
            int x = startX + dx;

            // Überspringe das Zentrum (das gegebene pTile selbst)
            if (dx == 0 && dy == 0) continue;

            // Stelle sicher, dass die Koordinaten innerhalb der Karte sind
            if (y >= 0 && y < m_arrTileMap.size() && x >= 0 && x < m_arrTileMap[0].size()) {
                Tile* neighbourTile = m_arrTileMap[y][x]; // Zugriff auf das Nachbartile

                // Versuche, die Einheit auf dem Tile als den gesuchten Gebäudetyp zu casten
                BuildingType* pBuilding = dynamic_cast<BuildingType*>(neighbourTile->getSeaOrLandUnit());

                // Wenn das Tile den gesuchten Gebäudetyp hat, gib diesen zurück
                if (pBuilding) {
                    return pBuilding;
                }
            }
        }
    }

    // Wenn kein passendes Gebäude gefunden wurde, gib nullptr zurück
    return nullptr;
}
