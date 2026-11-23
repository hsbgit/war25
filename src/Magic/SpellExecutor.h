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

#include "Magic/Runes.h"
#include "EventHandling/EventProcessor.h"
#include "Types.h"

class Unit;

class SpellExecutor : public EventProcessor {
public:
	SpellExecutor();
	~SpellExecutor();

	void process();

	void handleEvent(const Event& event);
	// Returns true only if the target/range was valid, mana was paid and the
	// effect was created. A null caster is reserved for the debug spell window.
	bool castSpell(Unit* caster, EventID spell, const Point& targetPos_world);
	bool castDebugSpell(EventID spell, const Point& targetPos_world);
	void setMap(Map* pMap);

	inline const std::vector<Runes>& getRunes() const {
		return vecRunes;
	}


private:
	void placeRunes(Runes& r, const Point& targetPos_world);
	bool castSpellInternal(Unit* caster, EventID spell,
		const Point& targetPos_world, bool debugCast);

	std::vector<Runes> vecRunes;
	Map* m_pMap = nullptr;
};


extern SpellExecutor gSpellExecutor;
