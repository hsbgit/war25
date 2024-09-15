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
	MiniMap::MiniMap(Map* pMap, Viewport* pViewPort) : ImGuiWindow(), m_pMap(pMap), m_pViewPort(pViewPort), m_width(m_pMap->getMiniMapWithOverlay()->getWidth()), m_height(m_pMap->getMiniMapWithOverlay()->getHeight()) {
		m_scale_x = (float)m_width / (float)(m_pMap->sideLength() * Tile::TileWidth_px);
		scale_y = (float)m_height / (float)(m_pMap->sideLength() * Tile::TileWidth_px);
	}


	void MiniMap::draw() {
		ImGui::Begin("MiniMap");

		gui::renderImage(m_pMap->getMiniMapWithOverlay());

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsItemHovered() && io.MouseClicked[0]) {
			ImVec2 clicked_global = ImGui::GetMousePos();

			// It's a bit hacked:
			// Header (with bar to grap) has approx. 28 pixels
			// The side to the left is approx. 10 pixels            
			Point clicked_local = { (int)clicked_global.x - (int)ImGui::GetWindowPos().x - 10, (int)clicked_global.y - (int)ImGui::GetWindowPos().y - 28 };

			// Scale up
			clicked_local.x = (int)((float)clicked_local.x / m_scale_x);
			clicked_local.y = (int)((float)clicked_local.y / scale_y);

			// Convert from [px_world] to [tile_world]                      
			m_pViewPort->setViewPortPosition(m_pViewPort->pixelToTile(clicked_local));
		}

		ImGui::End();
	}
}