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
#include "StateDead.h"
#include "Units/Unit.h"
#include "Map/Map.h"
#include "Resources/resourcemanager_wartools.h"
#include <iostream>


StateDead::StateDead(Object* pOwner) : State(pOwner) {

}


void StateDead::onEnter(const Event* pEvent) {
	std::cerr << "Entered StateDead" << std::endl;
}


void StateDead::onExit() {

}


State* StateDead::process() {
	return this;
}
