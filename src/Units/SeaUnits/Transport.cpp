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
#include "Transport.h"
#include "Resources/resourcemanager_wartools.h"
#include "Player.h"
#include "Map/Map.h"
#include "EventHandling/EventBroker.h"
#include "Global/GlobalAttributes.h"
#include "Resources/SoundManager.h"
#include <algorithm>
#include <limits>


Transport::Transport(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 150, 0, 4, 10) {
	// Generic behavior
	if (m_fraction == Fraction::Alliance) {
		setSoundBaseDirectory("human/ships/");
	}
	else {
		setSoundBaseDirectory("orc/ships/");
	}

	setDyingSound("ships/sinking.wav");

	// Specific behavior
	addAllowedUpgrade(Upgrade::ItemType::Foundry_Ship_Armor_1);
	addAllowedUpgrade(Upgrade::ItemType::Foundry_Ship_Armor_2);

	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_HUMAN_TRANSPORT, this->getFraction(), getOwner()->getColor());
	setName("Transport");
}

Tile* Transport::findBoardingApproach(const Unit* passenger,
	EventProcessorID ignoredReservation) {
	if (!passenger || !isPlacedOnMap()) return nullptr;
	const int unitSize = gMapUnitTilesize.at(typeid(*passenger));
	const Point passengerPos = passenger->getTileLocation();
	const int minX = std::max(0, getTileTopLeft()->getX() - unitSize);
	const int minY = std::max(0, getTileTopLeft()->getY() - unitSize);
	const int maxX = std::min(getMap()->sideLength() - 1,
		getTileLowerRight()->getX() + unitSize);
	const int maxY = std::min(getMap()->sideLength() - 1,
		getTileLowerRight()->getY() + unitSize);
	Tile* best = nullptr;
	int bestDistance = std::numeric_limits<int>::max();
	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			if (!getMap()->isSpaceAvailableForUnit(y, x, unitSize, passenger->getType()))
				continue;
			const bool reserved = std::any_of(m_pendingPassengers.begin(),
				m_pendingPassengers.end(), [x, y, ignoredReservation](const PendingPassenger& pending) {
					return pending.id != ignoredReservation &&
						pending.approach.x == x && pending.approach.y == y;
				});
			if (reserved) continue;
			const int dx = passengerPos.x - x;
			const int dy = passengerPos.y - y;
			const int distance = dx * dx + dy * dy;
			if (distance < bestDistance) {
				bestDistance = distance;
				best = getMap()->getTile({x, y});
			}
		}
	}
	return best;
}

Tile* Transport::requestBoard(Unit* passenger) {
	if (!passenger || passenger == this || passenger->getOwner() != getOwner() ||
		!passenger->isLandObject() || passenger->isDead() ||
		m_passengers.size() + m_pendingPassengers.size() >= Capacity)
		return nullptr;
	const EventProcessorID id = passenger->getID();
	if (std::find(m_passengers.begin(), m_passengers.end(), id) != m_passengers.end())
		return nullptr;
	if (auto existing = std::find_if(m_pendingPassengers.begin(), m_pendingPassengers.end(),
		[id](const PendingPassenger& pending) { return pending.id == id; });
		existing != m_pendingPassengers.end())
		return getMap()->getTile(existing->approach);
	Tile* approach = findBoardingApproach(passenger);
	if (!approach) return nullptr;
	m_pendingPassengers.push_back({id, approach->getPos()});
	return approach;
}

void Transport::cancelBoarding(const Unit* passenger) {
	if (!passenger)
		return;
	const EventProcessorID id = passenger->getID();
	std::erase_if(m_pendingPassengers,
		[id](const PendingPassenger& pending) { return pending.id == id; });
}

bool Transport::isDocked() const {
	if (!isPlacedOnMap()) return false;
	return std::any_of(getOccupiedTiles().begin(), getOccupiedTiles().end(),
		[](const Tile* tile) { return tile->isCoast(); });
}

std::vector<Unit*> Transport::loadedPassengers() const {
	std::vector<Unit*> passengers;
	passengers.reserve(m_passengers.size());
	for (EventProcessorID id : m_passengers) {
		if (auto* passenger = dynamic_cast<Unit*>(g_pEventBroker.getEventHandler(id));
			passenger && !passenger->isDead())
			passengers.push_back(passenger);
	}
	return passengers;
}

void Transport::process() {
	Unit::process();
	if (!isPlacedOnMap() || isDead()) return;
	const bool docked = isDocked();
	if (m_dockStateInitialized && docked && !m_wasDocked)
		g_pSoundManager->playWAV(this, "misc/transport_docking.wav");
	m_wasDocked = docked;
	m_dockStateInitialized = true;
	for (auto it = m_pendingPassengers.begin(); it != m_pendingPassengers.end();) {
		auto* passenger = dynamic_cast<Unit*>(g_pEventBroker.getEventHandler(it->id));
		if (!passenger || passenger->isDead() || passenger->getOwner() != getOwner()) {
			it = m_pendingPassengers.erase(it);
			continue;
		}
		if (!passenger->isPlacedOnMap()) {
			it = m_pendingPassengers.erase(it);
			continue;
		}
		const Point ship = getTileLocation();
		const Point unit = passenger->getTileLocation();
		const int size = gMapUnitTilesize.at(typeid(*this));
		const int dx = std::max({ship.x - unit.x, 0, unit.x - (ship.x + size - 1)});
		const int dy = std::max({ship.y - unit.y, 0, unit.y - (ship.y + size - 1)});
		if (std::max(dx, dy) > 1) {
			Tile* approach = getMap()->getTile(it->approach);
			Object* occupant = approach ? approach->getSeaOrLandUnit() : nullptr;
			if (!approach || (occupant && occupant != passenger)) {
				approach = findBoardingApproach(passenger, it->id);
				if (approach) it->approach = approach->getPos();
			}
			// A command may have ended because the old approach became occupied.
			// Reissue only while idle; moving units handle temporary blockers themselves.
			if (approach && !passenger->isMoving())
				passenger->moveToBoardingApproach(approach);
			++it;
			continue;
		}
		passenger->handleEvent(Event(passenger->getID(), GeneralActions::Stop));
		getMap()->releaseObjectFromTile(passenger);
		m_passengers.push_back(it->id);
		it = m_pendingPassengers.erase(it);
	}
}

void Transport::unloadAll(const Point& preferredDestination) {
	if (!isPlacedOnMap()) return;
	for (auto it = m_passengers.begin(); it != m_passengers.end();) {
		auto* passenger = dynamic_cast<Unit*>(g_pEventBroker.getEventHandler(*it));
		if (!passenger || passenger->isDead()) {
			it = m_passengers.erase(it);
			continue;
		}
		Tile* landing = getMap()->getFreeTileAroundBuilding(this, passenger,
			preferredDestination);
		if (!landing) break;
		getMap()->placeUnit(landing, passenger);
		passenger->setIdleAnimation();
		it = m_passengers.erase(it);
	}
}

void Transport::handleEvent(const Event& event) {
	if (event == TransportActions::Unload) {
		unloadAll({event.tileX(), event.tileY()});
		return;
	}
	Unit::handleEvent(event);
}

std::vector<Action> Transport::getActions() const {
	std::vector<Action> actions = Unit::getActions();
	actions.push_back(Action(getID(), EventID(TransportActions::Unload),
		g_pRessourceManager->getIcon(PUD_ICON_HUMAN_UNLOAD, getFraction(), getOwner()->getColor()),
		Action::ActionType::WithWorldTile));
	return actions;
}

void Transport::doHandleDeath() {
	Unit::doHandleDeath();
	for (EventProcessorID id : m_passengers) {
		if (auto* passenger = dynamic_cast<Unit*>(g_pEventBroker.getEventHandler(id))) {
			passenger->removeBuff(BuffType::UnholyArmor);
			passenger->die();
		}
	}
	m_passengers.clear();
	m_pendingPassengers.clear();
}
