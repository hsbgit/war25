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

#include <set>
#include <string>
#include <cassert>
#include <typeinfo>
#include <typeindex>
#include <algorithm>
#include "Map/Tile.h"
#include "Global/Action.h"
#include "Units/IArmed.h"
#include "UpgradeEffect.h"
#include "Upgrade.h"
#include "Fractions.h"

class Map;
class Action;
class Player;
class Sprite;
class IArmed;
class ResourceManager;
class ResourceManager_wartools;



class Object {
public:
	Object(Player* pOwner, Map* pMap, int healthPointsMax, int armor, int sight);
	virtual ~Object();

	// Exemplary actions: Move, Stop, Attack, ...
	virtual std::vector<Action> getActions() const {
		std::vector<Action> vecEmpty;
		return vecEmpty;
	}

	virtual void installUpgrade(const Upgrade& upgrade) {
		if (m_setAllowedUpgrades.contains(upgrade.getItemType()) && !hasUpgrade(upgrade.getItemType())) {
			m_setInstalledUpgrades.insert(upgrade.getItemType());

			m_upgradeStats += upgrade.getUpgradeEffect();
		}
	}

	UpgradeEffect m_upgradeStats;

	bool hasUpgrade(Upgrade::ItemType u) const {
		return m_setInstalledUpgrades.contains(u);
	}

	void addAllowedUpgrade(Upgrade::ItemType u) {
		m_setAllowedUpgrades.insert(u);
	}

	std::set<Upgrade::ItemType> m_setAllowedUpgrades;
	std::set<Upgrade::ItemType> m_setInstalledUpgrades;


	virtual void die() {
		takeDamage(AttackerStats(0, 999999), true);
	}



	inline const std::string& getName() const {
		return m_name;
	}


	virtual void process() = 0;

	inline Player* getOwner() const {
		return m_pOwner;
	}

	// ToDo: Neutral, Allies,...
	bool isEnemy(Object* pPlayer) const;


	Fraction getFraction() const { return m_fraction; }

	enum class Type { Land, Sea, Air };

	virtual Type getType() const = 0;

	inline bool isLandObject() const {
		return getType() == Type::Land;
	}

	inline bool isSeaObject() const {
		return getType() == Type::Sea;
	}

	inline bool isAirObject() const {
		return getType() == Type::Air;
	}

	virtual bool isSelectable() const {
		return !isDead();
	}

	inline bool isDead() const {
		return (m_healthPoints <= 0);
	}


	virtual void takeDamage(const AttackerStats& stats, bool noRandomComponent = false);

	virtual void doHandleDeath() = 0;
	virtual bool isMoving() const = 0;


	// Several attackers might attack an Object at the same time.
	inline void registerAttacker(IArmed* pAttacker) {
		m_vecAttackers.push_back(pAttacker);
	}

	// If one attacker kills this object we have to notify all remaining attackers
	// that their active target is already dead.
	void notifyAttackersThatImDead();

	virtual const Sprite& sprite() const = 0;

	inline const Sprite& icon() const {
		assert(m_icon.getTexture());
		return m_icon;
	}

	// getTilePosition returns the top left tile of an unit. This is the real position if the unit is not moving
	// However, if the unit is currently moving from one tile to another there is some offset which can be
	// get with this function
	const Point& getTilePositionOffset() const {
		return m_tilePositionOffset_px;
	}



	// Returns the tile where this unit or building stands on.
	// For buildings or bigger units: pos() specifies the upper left tile of the structure
	inline const Point& getTileLocation() const {
		return getTileTopLeft()->getPos();
	}

	inline Tile* getTile() const {
		return getTileTopLeft();
	}



	virtual int getArmor() const {
		return m_armor;
	}

	virtual int getSight() const {
		return m_sight;
	}

	// used by Troll Beserker or Paladin healing spell
	inline void increaseHealth(int hp) {
		assert(!isDead());
		m_healthPoints = std::min(m_healthPoints + hp, m_healthPointsMax);
	}

	inline int getHealthPoints() const {
		return std::max(0, m_healthPoints);
	}

	inline int getMaxHealthPoints() const {
		return m_healthPointsMax;
	}

	inline float getRelativeHealth() const {
		return (float)getHealthPoints() / (float)getMaxHealthPoints();
	}


	// funktion center? für beschuss und brennendes feuer?

	// TL will be LR if the building/unit size is 1x1.
	inline Tile* getTileTopLeft() const {
		assert(isPlacedOnMap());
		return m_vecOccupiedTiles.front();
	}

	inline Tile* getTileLowerRight() const {
		assert(isPlacedOnMap());
		return m_vecOccupiedTiles.back();
	}

	inline bool isPlacedOnMap() const {
		return m_vecOccupiedTiles.size();
	}

	inline Map* getMap() {
		return m_pMap;
	}

	// If the unit size is bigger than 1x1 the first element will always be the top left tile,
	// the last element will always be the lower right tile due to the implementation in
	// Map::placeUnit()    
	inline std::vector<Tile*>& getOccupiedTiles() {
		return m_vecOccupiedTiles;
	}


private:
	std::vector<Tile*> m_vecOccupiedTiles;

protected:
	void setName(const std::string& name) {
		m_name = name;
	}

	void setHealthPointsMax(int newHealthPointsMax) {
		int diff = newHealthPointsMax - m_healthPointsMax;
		m_healthPoints += diff;
		m_healthPointsMax = newHealthPointsMax;
	}

	void setSightRange(int newSightRange) {
		m_sight = newSightRange;
	}

	Sprite m_icon;
	Player* m_pOwner = nullptr;

	int m_sight;
	int m_armor;
	int m_healthPoints, m_healthPointsMax;

	Map* m_pMap = nullptr;


	Fraction m_fraction = Fraction::Neutral; // todo: protected? setter?

public:
	void onClicked();
	virtual void playSoundAcknowledge() { }; // Empty by default

protected:
	virtual void playSoundSelected() = 0;


	inline void setDyingSound(const std::string& strPath) {
		m_strDyingSound = strPath;
	}


public:
	void setTilePositionOffset(const Point& tilePositionOffset_px) {
		m_tilePositionOffset_px = tilePositionOffset_px;
	}

	void clearTilePositionOffset() {
		m_tilePositionOffset_px = { 0,0 };
	}

private:
	void setDeathProperties();

	std::string m_strDyingSound; // Must be set in sublcasses

protected:
	Point m_tilePositionOffset_px = { 0,0 };

private:
	std::string m_name;
	std::vector<IArmed*> m_vecAttackers;
};

