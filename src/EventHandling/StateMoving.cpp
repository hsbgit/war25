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


#include "StateMoving.h"
#include "Map/AStar.h"
#include "Map/Tile.h"
#include "Map/Map.h"

#include <map>
#include <iostream>



Tile* StateMoving::isTargetTileANeighbourTile() {
	auto& neighbourTiles = m_pOwner->getTileTopLeft()->neighbourTiles_SameType;
	auto it = std::find_if(neighbourTiles.begin(), neighbourTiles.end(), [this](const auto& it) {
		return it.second == m_pTargetTile;
		});

	return (it != neighbourTiles.end()) ? dynamic_cast<Tile*>((*it).second) : nullptr;
}


void StateMoving::setDirection(Direction direction) {
	if (direction != m_currentDirection) {
		m_directionChanged = true;
		m_currentDirection = direction;
	}
	else {
		m_directionChanged = false;
	}
}


Tile* StateMoving::getNextPathTile(bool* pathTemporarilyBlocked) {
	*pathTemporarilyBlocked = false;
	assert(m_path.isValid());

	Tile* pNextTile = dynamic_cast<Tile*>(m_path.getFront());

	Object* pBlockingObject = nullptr;
	if (pNextTile->isBlockedForUnit(m_pOwner, &pBlockingObject)) {
		if (pBlockingObject && pBlockingObject->isMoving())
			*pathTemporarilyBlocked = true;

		return nullptr;
	}

	setDirection(getTargetCardinalDirection(m_pOwner->getTile(), pNextTile));
	m_path.popFront();
	return pNextTile;
}


Tile* StateMoving::getNextTile(bool* pathTemporarilyBlocked) {
	assert(pathTemporarilyBlocked);
	*pathTemporarilyBlocked = false;

	// Is the next tile the final target tile?
	Tile* pTargetTile = isTargetTileANeighbourTile();
	if (pTargetTile) {
		// Check whether target tile is blocked -> if yes, we stop as we are as closest as possible
		if (!m_pOwner->getMap()->canBePlacedAtPosition(m_pOwner, pTargetTile->getX(), pTargetTile->getY()))
			return nullptr;

		assert(pTargetTile->getSeaOrLandUnit() == nullptr);
		setDirection(getTargetCardinalDirection(m_pOwner->getTile(), pTargetTile));
		return pTargetTile;
	}

	// Is a path used?
	if (m_path.isValid())
		return getNextPathTile(pathTemporarilyBlocked);


	// Otherwise use a simple approach: Simply walk into the target direction and avoid 1x1 obstacles if possible
	Point currPos_tile = m_pOwner->getTileLocation();
	Direction targetDir = getTargetCardinalDirection(currPos_tile, m_targetPos_world);


	int unitSize_tile = (m_pOwner->isAirObject() || m_pOwner->isSeaObject()) ? 2 : 1;

	auto mapFreeTiles = m_pOwner->getTileTopLeft()->getFreeNeighbourNodes(unitSize_tile, m_pOwner->isAirObject());
	if (mapFreeTiles.contains(targetDir)) {
		setDirection(targetDir);
		return dynamic_cast<Tile*>(mapFreeTiles[targetDir]);
	}

	// Direct tile is blocked, maybe the left or right tile around it is free.
	// Example: SE is not available but S or E may be.
	int dir = static_cast<int>(targetDir) - 1;
	if (dir < 0) { // targetDir was N(0) -> -1 -> must map to NW
		dir = static_cast<int>(Direction::NW);
	}

	Direction left = static_cast<Direction>(dir);
	if (mapFreeTiles.contains(left)) {
		setDirection(left);
		return dynamic_cast<Tile*>(mapFreeTiles[left]);
	}


	dir = static_cast<int>(targetDir) + 1;
	if (dir > 7) { // targetDir was NW(7) -> +1 -> must map to N
		dir = static_cast<int>(Direction::N);
	}

	Direction right = static_cast<Direction>(dir);
	if (mapFreeTiles.contains(right)) {
		setDirection(right);
		return dynamic_cast<Tile*>(mapFreeTiles[right]);
	}

	// Simple approach (straight,left, right) did not work, we are blocked
	// Let's try AStar
	assert(!m_path.isValid());  // error if target could not be reached
	assert(m_pTargetTile);

	AStar a(dynamic_cast<INode*>(m_pOwner->getTileTopLeft()), dynamic_cast<INode*>(m_pTargetTile), unitSize_tile, m_pOwner->isAirObject());
	m_path = a.findPath();

	if (m_path.isValid()) {
		return getNextPathTile(pathTemporarilyBlocked);
	}

	// Well, no way/path found
	return nullptr;
}





#include "Map/AStar.h"
#include "Map/Path.h"
#include "Global/Object.h"
#include "Units/Unit.h"



void StateMoving::onEnter(const Event* pEvent) {
	if (pEvent) {
		// Todo: replace Point with own structure
		Point targetPos_world = { pEvent->tileX(), pEvent->tileY() };
		setTargetTile(targetPos_world);
	}
}


void StateMoving::setTargetTile(const Point& targetPos_world) {
	m_targetPos_world = targetPos_world;
	m_pTargetTile = m_pOwner->getMap()->getTile(m_targetPos_world);
}


void StateMoving::onExit() {
	// Update animation according to direction
	Unit* pUnit = dynamic_cast<Unit*>(m_pOwner);
	pUnit->setIdleAnimation();
	pUnit->clearTilePositionOffset();

	m_pNextTileToMove = nullptr;
	m_directionChanged = false;

	m_path.clear();

	State::onExit();
}


#include "Global/GlobalAttributes.h"
StateMoving::StateMoving(Object* pOwner) : State(pOwner) {
	m_ticksPerStep = calculateRequiredTicksForGameAction(250);
}


State* StateMoving::process() {
	Point currPos_tile = m_pOwner->getTileLocation();

	// Todo: write comparision operator
	// It is important to check whether m_pTileToMove is a nullptr
	// We set the next tile always in advance, so the last animation would not run to the end.
	// Hence, we have to wait until 'm_pTileToMove' is nullptr -> see end of this method
	if ((currPos_tile == m_targetPos_world) && !m_pNextTileToMove) {
		return transitionToNextState();
	}


	if (!m_pNextTileToMove) {
		bool temporarilyBlocked;
		m_pNextTileToMove = getNextTile(&temporarilyBlocked);

		if (!m_pNextTileToMove) {
			if (temporarilyBlocked)
				return this;
			else
				return transitionToNextState();
		}

		// Place/block tile in advance
		m_pOwner->getMap()->placeUnit(m_pNextTileToMove, m_pOwner);

		m_tilePositionOffset_px.x = -(m_pNextTileToMove->getX() - currPos_tile.x) * Tile::TileWidth_px;
		m_tilePositionOffset_px.y = -(m_pNextTileToMove->getY() - currPos_tile.y) * Tile::TileWidth_px;
		m_pOwner->setTilePositionOffset(m_tilePositionOffset_px);

		// Update animation according to direction
		Unit* pUnit = dynamic_cast<Unit*>(m_pOwner);

		if (m_directionChanged) {
			pUnit->setCurrentAnimation(g_pRessourceManager->getUnitAnimation(pUnit, m_currentDirection, ResourceManager_wartools::AnimationType::Moving));
		}
	}

	/*
	if (m_tickCounter++ != m_ticksPerStep) {
		return this;
	}

	m_tickCounter = 0;*/


	Point dest = m_pNextTileToMove->getPos();

	// 10 -> 1 tile pro sekunde ?
	Unit* pUnit = dynamic_cast<Unit*>(m_pOwner);
	// Deklaration der Richtungsabhängigen Geschwindigkeit vorab
	int speed_x = 0;
	int speed_y = 0;

	// Berechne die Richtung und die entsprechende Geschwindigkeit in x- und y-Richtung
	switch (m_currentDirection) {
	case Direction::N:
		speed_y = -pUnit->getSpeed();
		break;
	case Direction::S:
		speed_y = pUnit->getSpeed();
		break;
	case Direction::W:
		speed_x = -pUnit->getSpeed();
		break;
	case Direction::E:
		speed_x = pUnit->getSpeed();
		break;
	case Direction::NW:
		speed_x = -pUnit->getSpeed();
		speed_y = -pUnit->getSpeed();
		break;
	case Direction::NE:
		speed_x = pUnit->getSpeed();
		speed_y = -pUnit->getSpeed();
		break;
	case Direction::SW:
		speed_x = -pUnit->getSpeed();
		speed_y = pUnit->getSpeed();
		break;
	case Direction::SE:
		speed_x = pUnit->getSpeed();
		speed_y = pUnit->getSpeed();
		break;
	}

	// Anpassung der Position auf dem Tile basierend auf der Geschwindigkeit
	m_tilePositionOffset_px.x += speed_x;
	m_tilePositionOffset_px.y += speed_y;
	m_pOwner->setTilePositionOffset(m_tilePositionOffset_px);

	// Überprüfung, ob das Ziel erreicht wurde (Übergang des Vorzeichens)
	if (sgn(m_tilePositionOffset_px.x) != sgn(speed_x) || sgn(m_tilePositionOffset_px.y) != sgn(speed_y)) {
		m_tilePositionOffset_px = { 0, 0 };
		m_pOwner->clearTilePositionOffset();
		m_pNextTileToMove = nullptr; // Nächstes Ziel-Tile zurücksetzen, da das aktuelle Ziel erreicht wurde
	}

	return this;
}





