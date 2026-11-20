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
#include <functional>

class Map;
class Viewport;

namespace gui {
	class MiniMap : public ImGuiWindow {
	public:
		using WorldCommandCallback = std::function<void(const Point&)>;
		MiniMap(Map* pMap, Viewport* pViewPort, WorldCommandCallback worldCommandCallback = {});

		void draw() override;

	private:
		Map* m_pMap = nullptr;
		Viewport* m_pViewPort = nullptr;
		int m_width, m_height;
		WorldCommandCallback m_worldCommandCallback;
	};
}
