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

#include "Window.h"
class Map;

namespace gui {
	class DebugSpellWindow : public ImGuiWindow {
	public:
		DebugSpellWindow() {

		}

		inline void setMap(Map* pMap) {
			m_pMap = pMap;
		}

		void onLeftClicked(const Point& tile_world);

		void draw() override;

	private:
		Map* m_pMap = nullptr;

		int m_selectedSpell = -1;
		int mana = 255, maxMana = 255;
	};
}
