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


class SpellExecutor : public EventProcessor {
public:
	SpellExecutor();
	~SpellExecutor();

	void process();

	void handleEvent(const Event& event);
	void setMap(Map* pMap);

	inline const std::vector<Runes>& getRunes() const {
		return vecRunes;
	}


private:
	void placeRunes(Runes& r, const Point& targetPos_world);

	std::vector<Runes> vecRunes;
	Map* m_pMap = nullptr;
};


extern SpellExecutor gSpellExecutor;
