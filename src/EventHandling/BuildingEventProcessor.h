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

#include <queue>
#include <cassert>
#include <cstdint>
#include <vector>

#include "Sprite.h"
#include "Global/Fractions.h"
#include "Global/ProductionItem.h"
#include "EventProcessor.h"
#include "Global/Object.h"

class Map;


class BuildingEventProcessor : public EventProcessor {
public:
	BuildingEventProcessor(Object* pObject, Fraction buildingFraction, Map* pMap);
	virtual ~BuildingEventProcessor();

	void handleEvent(const Event& event) override;

	virtual const Sprite& getCurrentEventIcon() const {
		return m_currEventInProgressIcon;
	}

	const std::vector<Sprite>& getQueuedEventIcons() const {
		return m_vecQueuedEventIcons;
	}

	virtual float getRelativeProgressOfCurrentEvent() const {
		assert(isEventInProgress());
		return float(m_tickCnt) / float(m_requiredEventTicks);
	}


protected:
	void doEventHandling();
	void queueEvent(const Event& event) {}

private:
	bool eventFinished(const Event& event);
	bool createUnit(ProductionItem::ItemType unitType);

	Object::Type getUnitDomain(ProductionItem::ItemType unitType);
	Object* createUnitObject(ProductionItem::ItemType unitType);

	Sprite m_currEventInProgressIcon;
	ProductionItem m_currEventInProgressItem;
	std::vector<Sprite> m_vecQueuedEventIcons;
	std::queue<ProductionItem> m_vecQueuedProductionItems;

	inline Object* getObject() {
		return m_pObject;
	}

	inline void addToEventQueue(const Event& event) {
		m_eventQueue.push(event);
	}

	inline bool hasQueuedEvents() const {
		return m_eventQueue.size();
	}

	int m_tickCnt = 0;
	int m_requiredEventTicks = 0;

	Object* m_pObject = nullptr;
	Player* m_pObjectOwner = nullptr;

	std::queue<Event> m_eventQueue;
	static const std::size_t maxEventQueueLength;

	Map* m_pMap;
	Fraction m_buildingFraction;
};
