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
#include "MiniMap.h"
#include "HelperFunctions.h"
#include "Map/Map.h"
#include "Global/Viewport.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include <algorithm>


namespace gui {
	MiniMap::MiniMap(Map* pMap, Viewport* pViewPort, WorldCommandCallback worldCommandCallback)
		: ImGuiWindow(), m_pMap(pMap), m_pViewPort(pViewPort),
		  m_width(m_pMap->getMiniMapWithOverlay()->getWidth()),
		  m_height(m_pMap->getMiniMapWithOverlay()->getHeight()),
		  m_worldCommandCallback(std::move(worldCommandCallback)) {}


	void MiniMap::draw() {
		update();
		ImGui::Begin("MiniMap");

		gui::renderImage(m_pMap->getMiniMapWithOverlay());

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsItemHovered() && (io.MouseClicked[0] || io.MouseClicked[1])) {
			const ImVec2 imageMin = ImGui::GetItemRectMin();
			const ImVec2 imageSize = ImGui::GetItemRectSize();
			const ImVec2 mouse = ImGui::GetMousePos();
			Point worldTile{
				std::clamp(static_cast<int>((mouse.x - imageMin.x) / imageSize.x * m_pMap->sideLength()), 0, m_pMap->sideLength() - 1),
				std::clamp(static_cast<int>((mouse.y - imageMin.y) / imageSize.y * m_pMap->sideLength()), 0, m_pMap->sideLength() - 1)
			};

			if (io.MouseClicked[0])
				m_pViewPort->setViewPortPosition(worldTile);
			else if (m_worldCommandCallback)
				m_worldCommandCallback(worldTile);
		}

		ImGui::End();
	}
}
