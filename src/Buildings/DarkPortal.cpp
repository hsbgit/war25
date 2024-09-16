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
#include "DarkPortal.h"
#include "Resources/resourcemanager_wartools.h"


DarkPortal::DarkPortal(Player* pOwner, Map* pMap) : Building(pOwner, Fraction::Neutral, pMap, 5000, 0, 0) {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_DARK_PORTAL);
	m_sprite = g_pRessourceManager->getNeutralBuildingSprite(this);

	setName("Dark Portal");
}
