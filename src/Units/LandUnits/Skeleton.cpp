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
#include "Skeleton.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"


// Hm... has no fraction, neither human nor orc
Skeleton::Skeleton(Player* pOwner, Map* pMap) : Unit(pOwner, Fraction::Orc, pMap, 40, 0, 3, 8), IArmed(6, 3, 1) {
	// neutral\units\skeleton\acknowledgement 1 wav
	// oger punching?

	// sounds\missiles  punch.wav

	/// sounds\misc explosion

	setDyingSound("orc/basic_voices/dead.wav");
	setSoundBaseDirectory("orc/units/goblin_sappers/");
	setName("Skeletons");

	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_SKELETON, this->getFraction(), getOwner()->getColor());
}


void Skeleton::process() {
	/*
	m_lifespan_ms -= passedTime_ms;
	if (!isDead() && m_lifespan_ms < 0) {
		die();
		return;
	}
	*/
}
