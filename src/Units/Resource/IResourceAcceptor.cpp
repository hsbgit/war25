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
#include "IResourceAcceptor.h"
#include "IResourceCollector.h"
#include "Units/Unit.h"
#include "Player.h"
#include "Units/Resource/ResourceRules.h"
#include <iostream>
#include <limits>


IResourceAcceptor* findNearestResourceAcceptor(Unit* pUnit, Player* pPlayer) {
	IResourceCollector* pCollector = dynamic_cast<IResourceCollector*>(pUnit);
	assert(pCollector);

	IResourceAcceptor* pAcceptor = nullptr;
	int distance = std::numeric_limits<int>::max();

	for (auto& unit : pPlayer->getObjects()) {
		auto* pPossibleAcceptor = dynamic_cast<IResourceAcceptor*>(unit);
		const bool accepts = pPossibleAcceptor &&
			pPossibleAcceptor->acceptsResource(pCollector->currentlyCarrying());
		if (!ResourceRules::eligibleAcceptor(unit != nullptr,
			unit && !unit->isDead(), unit && unit->isPlacedOnMap(), accepts))
			continue;

		int newDistance = std::abs(unit->getTileLocation().x - pUnit->getTileLocation().x) + std::abs(unit->getTileLocation().y - pUnit->getTileLocation().y);
		if (!pAcceptor) {
			pAcceptor = pPossibleAcceptor;
			distance = newDistance;
		}
		else if (newDistance < distance) {
			distance = newDistance;
			pAcceptor = pPossibleAcceptor;
		}

	}

	if (!pAcceptor)
		std::cerr << "findNearestResourceAcceptor: No valid resource acceptor found!" << std::endl;

	return pAcceptor;
}
