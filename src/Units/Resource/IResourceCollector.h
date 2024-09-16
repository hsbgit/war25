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

#include "ResourceType.h"

class Player;


class IResourceCollector {
public:
	inline void setCarriedResource(ResourceType type) {
		m_carriedResources = type;
	}

	ResourceType currentlyCarrying() const {
		return m_carriedResources;
	}

	virtual bool canCollectResource(ResourceType t) const = 0;

protected:
	Player* m_pPlayer = nullptr;
	ResourceType m_carriedResources = ResourceType::Nothing;
};
