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
#include "Critter.h"
#include "Player.h"
#include "Global/PseudoRandom.h"
#include "Global/GlobalAttributes.h"
#include "Resources/resourcemanager_wartools.h"
#include <iostream>


/*
 * Critter is a bit special:
 *  - Is neutral -> PlayerColor::Neutral
 *  - No animation, just a single sprite for each direction
 */
Critter::Critter(Player* pOwner, Map* pMap) : Unit(pOwner, Fraction::Neutral, pMap, 5, 0, 2, 3), m_sprite(g_pRessourceManager->getCritterSprite(Direction::N)) {
	int adjustedTime_ms = (PseudoRandom::rnd(800, 2000) / g_tick_ms) * g_tick_ms; // Ensure that the random number can be divided by g_tick_ms without reminder
	m_requiredTicksPerMove = calculateRequiredTicksForGameAction(adjustedTime_ms);

	m_icon = g_pRessourceManager->getIcon(PUD_ICON_CRITTER_SHEEP);
	g_pRessourceManager->registerSeasonObserver(this);
}


void Critter::process() {
	if (isDead()) {
		m_currState = State::Dead;
	}

	switch (m_currState) {
	case State::Idle:
		stateIdle();
		break;

	case State::Moving:
		stateMoving();
		break;

	case State::Dead:
		stateDead();
		break;

	default:
		assert(0);
	}
}


void Critter::notifySeasonChanged(Season newSeason) {
	m_vecSoundsSelected.clear();
	m_vecSoundsAnnoyed.clear();

	switch (newSeason) {
	case Season::Summer:
		setName("Sheep");
		m_vecSoundsSelected.push_back("neutral/units/sheep/selected/1.wav");
		setDyingSound("neutral/units/sheep/selected/1.wav");
		break;

	case Season::Winter:
		setName("Seal");
		m_vecSoundsSelected.push_back("neutral/units/seal/selected/1.wav");
		setDyingSound("neutral/units/seal/annoyed/1.wav");
		break;

	case Season::Wasteland:
		setName("Pig");
		m_vecSoundsSelected.push_back("neutral/units/pig/selected/1.wav");
		setDyingSound("neutral/units/pig/annoyed/1.wav");
		break;

	case Season::Swamp:
		setName("Warthog");
		m_vecSoundsSelected.push_back("neutral/units/warthog/selected/1.wav");
		setDyingSound("neutral/units/warthog/annoyed/1.wav");
		break;

	default:
		break;
	}
}


void Critter::doHandleDeath() {
	m_currState = State::Dead;
	m_sprite = g_pRessourceManager->getCritterSprite(Direction::N, true); // direction does not matter, all are equal
	g_pRessourceManager->deregisterSeasonObserver(this);
}


void Critter::stateIdle() {
	/*
	 * Try to move each second to a neighbour tile. If there is no free tile, the
	 * critter stays where it is and tries again after m_timer elapsed
	 */
	if (++m_tickCnt == m_requiredTicksPerMove) {
		NeighbourNodes freeTiles = getTileTopLeft()->getFreeNeighbourNodes();

		if (freeTiles.size()) {
			m_currState = State::Moving;

			auto it = freeTiles.begin();
			std::advance(it, PseudoRandom::rnd(0, (int)freeTiles.size() - 1));
			direction = it->first;
			pTileToMove = dynamic_cast<Tile*>(it->second);

			Point oldPos_tile = getTileLocation(); // Save this for later usage: We have to calculate the difference between old position and new position set by 'placeUnit'

			Tile* pTile = pTileToMove;
			getMap()->placeUnit(pTile, this);

			m_tilePositionOffset_px.x = -(pTile->getX() - oldPos_tile.x) * Tile::TileWidth_px;
			m_tilePositionOffset_px.y = -(pTile->getY() - oldPos_tile.y) * Tile::TileWidth_px;

			m_sprite = g_pRessourceManager->getCritterSprite(direction);
		}
		else {
			// At least move by changing direction
			//this->getdiection ?? danach -- oder ++ um passend zu drehen
			direction = (Direction)PseudoRandom::rnd(0, 7);
			m_sprite = g_pRessourceManager->getCritterSprite(direction);

		}

		m_tickCnt = 0;
	}
}


// ToDo: Replace with c++ state machine pattern just like a regular unit? 
void Critter::stateMoving() {
	Tile* pTile = pTileToMove;
	Point dest = pTile->getPos();

	// 10 -> 1 tile per second?
	int speed_x = getSpeed();
	int speed_y = getSpeed();


	if (direction == Direction::N || direction == Direction::S) {
		speed_x = 0;

	}
	else if (direction == Direction::W || direction == Direction::E) {
		speed_y = 0;
	}

	if (direction == Direction::NW || direction == Direction::N || direction == Direction::NE) {
		speed_y = -speed_y;
	}

	if (direction == Direction::NW || direction == Direction::W || direction == Direction::SW) {
		speed_x = -speed_x;
	}

	int dt_dist_x_px = speed_x;
	int dt_dist_y_px = speed_y;

	// We are moving either in x or y direction or in both directions
	// To see whether our sprite reached the actual position (which is already blocked respectively occupied) we
	// check whether the sign switches
	int sign_x = sgn(m_tilePositionOffset_px.x);
	int sign_y = sgn(m_tilePositionOffset_px.y);

	m_tilePositionOffset_px.x += dt_dist_x_px;
	m_tilePositionOffset_px.y += dt_dist_y_px;


	if (sign_x != sgn(m_tilePositionOffset_px.x) || sign_y != sgn(m_tilePositionOffset_px.y)) {
		m_currState = State::Idle;
		m_tilePositionOffset_px = { 0,0 };
	}
}


void Critter::stateDead() {
	// Nothing to do, just vanishing...
	// 
	// sprite change
	// no longer selectable
	// isVanished timer
}


const Sprite& Critter::sprite() const {
	return m_sprite;
}
