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
#include "EventHandling/StateIdle.h"
#include "Global/GlobalAttributes.h"
#include "Units/Resource/IResourceCollector.h"

Tile* StateMoving::isTargetTileANeighbourTile() {
	const auto& neighbourTiles = m_pOwner->isAirObject()
		? m_pOwner->getTileTopLeft()->neighbourTiles
		: m_pOwner->getTileTopLeft()->neighbourTiles_SameType;
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

	if (!canMoveTo(pNextTile)) {
		*pathTemporarilyBlocked = isOccupiedByAnotherUnit(pNextTile);
		if (*pathTemporarilyBlocked) {
			// Usually keep the route through a chokepoint. Periodically discard it
			// so head-on traffic can select a free detour instead of waiting forever.
			if (++m_blockedPathRetries >= 5) {
				m_path.clear();
				m_blockedPathRetries = 0;
			}
			m_pathRetryTicks = calculateRequiredTicksForGameAction(100);
		} else {
			// Terrain may have changed since the route was calculated.
			m_path.clear();
		}
		return nullptr;
	}

	m_blockedPathRetries = 0;
	setDirection(getTargetCardinalDirection(m_pOwner->getTile(), pNextTile));
	m_path.popFront();
	return pNextTile;
}

bool StateMoving::canMoveTo(const Tile* tile) const {
	Map* map = m_pOwner->getMap();
	if (!map->canBePlacedAtPosition(m_pOwner, tile->getX(), tile->getY()))
		return false;
	const Tile* current = m_pOwner->getTileTopLeft();
	if (current->getX() == tile->getX() || current->getY() == tile->getY())
		return true;
	// A diagonal move needs clearance at both orthogonal intermediate
	// positions. This keeps 2x2 ships out of coast corners.
	return map->canBePlacedAtPosition(m_pOwner, tile->getX(), current->getY()) &&
		map->canBePlacedAtPosition(m_pOwner, current->getX(), tile->getY());
}

bool StateMoving::isOccupiedByAnotherUnit(const Tile* topLeft) const {
	const int footprint = gMapUnitTilesize.at(typeid(*m_pOwner));
	auto positionIsOccupied = [this, footprint](int topLeftX, int topLeftY) {
		for (int y = topLeftY; y < topLeftY + footprint; ++y) {
			for (int x = topLeftX; x < topLeftX + footprint; ++x) {
			Tile* tile = m_pOwner->getMap()->getTile({x, y});
			if (!tile) continue;
			Object* occupant = m_pOwner->isAirObject()
				? tile->getAirUnit() : tile->getSeaOrLandUnit();
			if (occupant && occupant != m_pOwner)
				return true;
			}
		}
		return false;
	};
	if (positionIsOccupied(topLeft->getX(), topLeft->getY()))
		return true;
	const Tile* current = m_pOwner->getTileTopLeft();
	if (current->getX() == topLeft->getX() || current->getY() == topLeft->getY())
		return false;
	return positionIsOccupied(topLeft->getX(), current->getY()) ||
		positionIsOccupied(current->getX(), topLeft->getY());
}


Tile* StateMoving::getNextTile(bool* pathTemporarilyBlocked) {
	assert(pathTemporarilyBlocked);
	*pathTemporarilyBlocked = false;
	if (m_pathRetryTicks > 0) {
		--m_pathRetryTicks;
		*pathTemporarilyBlocked = true;
		return nullptr;
	}

	// Is the next tile the final target tile?
	Tile* pTargetTile = isTargetTileANeighbourTile();
	if (pTargetTile) {
		if (!canMoveTo(pTargetTile)) {
			*pathTemporarilyBlocked = isOccupiedByAnotherUnit(pTargetTile);
			if (*pathTemporarilyBlocked)
				m_pathRetryTicks = calculateRequiredTicksForGameAction(100);
			return nullptr;
		}

		// canBePlacedAtPosition checks the correct occupancy layer and the full
		// footprint. Land/sea occupants below an air unit are intentionally valid.
		setDirection(getTargetCardinalDirection(m_pOwner->getTile(), pTargetTile));
		return pTargetTile;
	}

	// Is a path used?
	if (m_path.isValid())
		return getNextPathTile(pathTemporarilyBlocked);


	// Otherwise use a simple approach: Simply walk into the target direction and avoid 1x1 obstacles if possible
	Point currPos_tile = m_pOwner->getTileLocation();
	Direction targetDir = getTargetCardinalDirection(currPos_tile, m_targetPos_world);


	const int unitSize_tile = gMapUnitTilesize.at(typeid(*m_pOwner));

	auto mapFreeTiles = m_pOwner->getTileTopLeft()->getFreeNeighbourNodes(unitSize_tile, m_pOwner->isAirObject());
	if (mapFreeTiles.contains(targetDir) &&
		canMoveTo(dynamic_cast<Tile*>(mapFreeTiles[targetDir]))) {
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
	if (mapFreeTiles.contains(left) && canMoveTo(dynamic_cast<Tile*>(mapFreeTiles[left]))) {
		setDirection(left);
		return dynamic_cast<Tile*>(mapFreeTiles[left]);
	}


	dir = static_cast<int>(targetDir) + 1;
	if (dir > 7) { // targetDir was NW(7) -> +1 -> must map to N
		dir = static_cast<int>(Direction::N);
	}

	Direction right = static_cast<Direction>(dir);
	if (mapFreeTiles.contains(right) && canMoveTo(dynamic_cast<Tile*>(mapFreeTiles[right]))) {
		setDirection(right);
		return dynamic_cast<Tile*>(mapFreeTiles[right]);
	}

	// Simple approach (straight,left, right) did not work, we are blocked
	// Let's try AStar
	assert(!m_path.isValid());  // error if target could not be reached
	assert(m_pTargetTile);

	AStar a(dynamic_cast<INode*>(m_pOwner->getTileTopLeft()), dynamic_cast<INode*>(m_pTargetTile),
		unitSize_tile, m_pOwner->isAirObject(), m_pOwner);
	m_path = a.findPath();

	if (m_path.isValid()) {
		return getNextPathTile(pathTemporarilyBlocked);
	}

	// A* treats units as temporary congestion, so failure here means there is
	// no terrain-valid route to the requested destination.
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
	m_path.clear();
	m_pathRetryTicks = 0;
	m_blockedPathRetries = 0;
}


void StateMoving::onExit() {
	// Update animation according to direction
	Unit* pUnit = dynamic_cast<Unit*>(m_pOwner);
	pUnit->setIdleAnimation(m_currentDirection);
	if (auto* idle = dynamic_cast<StateIdle*>(pUnit->m_pIdle))
		idle->setFacing(m_currentDirection);
	pUnit->clearTilePositionOffset();

	m_pNextTileToMove = nullptr;
	m_directionChanged = false;
	m_animationNeedsUpdate = true;

	m_path.clear();
	m_blockedPathRetries = 0;

	State::onExit();
}


StateMoving::StateMoving(Object* pOwner) : State(pOwner) {
	m_ticksPerStep = calculateRequiredTicksForGameAction(250);
	m_ticksUntilPathCheck = calculateRequiredTicksForGameAction(2 * 1000);
}


State* StateMoving::process() {
	// Workers and ships periodically discard their cached route. This lets
	// resource collectors benefit from newly built drop-off buildings and all
	// naval units react to map changes that open a shorter path.
	const bool shouldRecheckPath = dynamic_cast<IResourceCollector*>(m_pOwner) ||
		m_pOwner->getType() == Object::Type::Sea;
	if (shouldRecheckPath && !m_pNextTileToMove &&
		--m_ticksUntilPathCheck <= 0) {
		m_path.clear();
		m_ticksUntilPathCheck = calculateRequiredTicksForGameAction(2 * 1000);
	}
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
			if (temporarilyBlocked) {
				// TODO: Implement proper yielding that doesn't break pathfinding
				return this; // Still blocked, wait
			}
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

		if (m_directionChanged || m_animationNeedsUpdate) {
			pUnit->setCurrentAnimation(g_pRessourceManager->getUnitAnimation(pUnit, m_currentDirection, ResourceManager_wartools::AnimationType::Moving));
			m_animationNeedsUpdate = false;
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
	// Deklaration der Richtungsabh�ngigen Geschwindigkeit vorab
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

	// �berpr�fung, ob das Ziel erreicht wurde (�bergang des Vorzeichens)
	if (sgn(m_tilePositionOffset_px.x) != sgn(speed_x) || sgn(m_tilePositionOffset_px.y) != sgn(speed_y)) {
		m_tilePositionOffset_px = { 0, 0 };
		m_pOwner->clearTilePositionOffset();
		m_pNextTileToMove = nullptr; // N�chstes Ziel-Tile zur�cksetzen, da das aktuelle Ziel erreicht wurde
	}

	return this;
}
