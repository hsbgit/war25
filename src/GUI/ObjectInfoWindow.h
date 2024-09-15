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
#include <set>

class Object;

namespace gui {
	class ObjectInfoWindow : public ImGuiWindow {
	public:
		ObjectInfoWindow(const std::set<Object*>& selectedObjects) : m_selectedObjects(selectedObjects) {

		}

		void onLeftClicked(const Point& tile_world);
		void onRightClicked(const Point& tile_world);

		inline bool isActionButtonActive() const {
			return m_selectedAction != -1;
		}

		void draw() override;

	private:
		void renderUnitInfo();
		void renderArmedInfo();
		void renderBuildingInfo();
		void renderBuildingProgressIndicatorAndEvents();
		void renderActions();

		int m_selectedAction = -1;
		Object* m_pCurrSelectedObject = nullptr;
		const std::set<Object*>& m_selectedObjects; // Reference to keep everything automatically synchronized with 'm_selectedObjects' of class Game (dead units are removed etc.)
	};
}

