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
#include "Buff.h"
#include "Global/GlobalAttributes.h"
#include <cassert>


Buff::Buff() {
	// For std::map
}


Buff::Buff(BuffType t) : m_type(t) {
	switch (t) {
	case BuffType::UnholyArmor:
		m_originalLifespan_ticks = calculateRequiredTicksForGameAction(20 * 1000); // ToDo: Exact value unknown
		break;

	case BuffType::Invisibility:
		m_originalLifespan_ticks = calculateRequiredTicksForGameAction(20 * 1000); // ToDo: Exact value unknown
		// Todo: Or until this unit attacks
		break;

	case BuffType::Slow:
		m_originalLifespan_ticks = calculateRequiredTicksForGameAction(20 * 1000); // ToDo: Exact value unknown
		break;

	case BuffType::Haste:
		m_originalLifespan_ticks = calculateRequiredTicksForGameAction(20 * 1000); // ToDo: Exact value unknown
		break;

	case BuffType::Bloodlust:
		m_originalLifespan_ticks = calculateRequiredTicksForGameAction(20 * 1000); // ToDo: Exact value unknown
		break;


	default:
		assert(0);
	}

	m_lifespan_ticks = m_originalLifespan_ticks;
}


void Buff::process() {
	assert(m_type != BuffType::Uninitialized);

	if (isExpired())
		return;

	--m_lifespan_ticks;
}
