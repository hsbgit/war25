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
#include <iostream>


IResourceAcceptor* findNearestResourceAcceptor(Unit* pUnit, Player* pPlayer) {
	IResourceCollector* pCollector = dynamic_cast<IResourceCollector*>(pUnit);
	assert(pCollector);

	IResourceAcceptor* pAcceptor = nullptr;
	int distance = 99999; // todo: constant

	for (auto& unit : pPlayer->getObjects()) {
		IResourceAcceptor* pPossibleAcceptor = dynamic_cast<IResourceAcceptor*>(unit);

		if (dynamic_cast<IResourceAcceptor*>(pPossibleAcceptor) == nullptr)
			continue;


		if (!pPossibleAcceptor->acceptsResource(pCollector->currentlyCarrying()))
			continue;

		int newDistance = std::abs(unit->getTileLocation().x - pUnit->getTileLocation().x) + std::abs(unit->getTileLocation().y - pUnit->getTileLocation().y);
		if (!pAcceptor) {
			pAcceptor = pPossibleAcceptor;
			distance = newDistance;
		}
		else if (newDistance < distance) {
			distance = newDistance;
			// location
		}

	}

	if (!pAcceptor)
		std::cerr << "findNearestResourceAcceptor: No valid resource acceptor found!" << std::endl;

	return pAcceptor;
}

