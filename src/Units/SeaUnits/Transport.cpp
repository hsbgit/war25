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
#include <algorithm>


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

bool Transport::requestBoard(Unit* passenger) {
	if (!passenger || passenger == this || passenger->getOwner() != getOwner() ||
		!passenger->isLandObject() || passenger->isDead() ||
		m_passengers.size() + m_pendingPassengers.size() >= Capacity)
		return false;
	const EventProcessorID id = passenger->getID();
	if (std::find(m_passengers.begin(), m_passengers.end(), id) != m_passengers.end() ||
		std::find(m_pendingPassengers.begin(), m_pendingPassengers.end(), id) != m_pendingPassengers.end())
		return false;
	m_pendingPassengers.push_back(id);
	return true;
}

void Transport::process() {
	Unit::process();
	if (!isPlacedOnMap() || isDead()) return;
	for (auto it = m_pendingPassengers.begin(); it != m_pendingPassengers.end();) {
		auto* passenger = dynamic_cast<Unit*>(g_pEventBroker.getEventHandler(*it));
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
			++it;
			continue;
		}
		passenger->handleEvent(Event(passenger->getID(), GeneralActions::Stop));
		getMap()->releaseObjectFromTile(passenger);
		m_passengers.push_back(*it);
		it = m_pendingPassengers.erase(it);
	}
}

void Transport::unloadAll() {
	if (!isPlacedOnMap()) return;
	for (auto it = m_passengers.begin(); it != m_passengers.end();) {
		auto* passenger = dynamic_cast<Unit*>(g_pEventBroker.getEventHandler(*it));
		if (!passenger || passenger->isDead()) {
			it = m_passengers.erase(it);
			continue;
		}
		Tile* landing = getMap()->getFreeTileAroundBuilding(this, passenger);
		if (!landing) break;
		getMap()->placeUnit(landing, passenger);
		passenger->setIdleAnimation();
		it = m_passengers.erase(it);
	}
}

void Transport::handleEvent(const Event& event) {
	if (event == TransportActions::Unload) {
		unloadAll();
		return;
	}
	Unit::handleEvent(event);
}

std::vector<Action> Transport::getActions() const {
	std::vector<Action> actions = Unit::getActions();
	actions.push_back(Action(getID(), EventID(TransportActions::Unload),
		g_pRessourceManager->getIcon(PUD_ICON_HUMAN_UNLOAD, getFraction(), getOwner()->getColor())));
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
