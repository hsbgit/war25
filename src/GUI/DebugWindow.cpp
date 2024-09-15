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

#include "DebugWindow.h"
#include "Map/Tile.h"
#include "Global/DebugManager.h"


namespace gui {
	void DebugWindow::draw() {
		static bool show_another_window = true;

		if (!show_another_window)
			return;

		ImGui::Begin("Debug Features", &show_another_window);
		ImGui::Checkbox("Draw tile information", &gDebugManager.m_map_drawTileInformation);
		if (gDebugManager.m_map_drawTileInformation && m_pTile) {
			std::string strPos = "Tile Position (world): " + std::to_string(m_pTile->getX()) + "," + std::to_string(m_pTile->getY());

			std::string isCoast = "isCoast  : " + std::to_string(m_pTile->isCoast());
			std::string isLand = "isLand   : " + std::to_string(m_pTile->isLand());
			std::string isSea = "isSea    : " + std::to_string(m_pTile->isSea());
			std::string isLumber = "isLumber : " + std::to_string(m_pTile->isWood());
			std::string hasObject = "hasObject: " + std::to_string(m_pTile->hasObject());
			std::string isDestroyable = "isDestroyable: " + std::to_string(m_pTile->isDestroyable());


			ImGui::TextUnformatted(strPos.c_str());
			ImGui::TextUnformatted(isCoast.c_str());
			ImGui::TextUnformatted(isLand.c_str());
			ImGui::TextUnformatted(isSea.c_str());
			ImGui::TextUnformatted(isLumber.c_str());
			ImGui::TextUnformatted(hasObject.c_str());
			ImGui::TextUnformatted(isDestroyable.c_str());
		}
		ImGui::Checkbox("Consider exploration", &gDebugManager.m_map_considerExploration);
		ImGui::Checkbox("Use fog of war", &gDebugManager.m_map_drawFogOfWar);

		ImGui::Separator();
		ImGui::Text("Note: The following options are related to the last AStar execution!");
		ImGui::Text("Note: Debug with just one selected unit!");
		ImGui::Checkbox("A*: Render open list (blue)", &gDebugManager.m_astar_showOpenList);
		ImGui::Checkbox("A*: Render closed list (red)", &gDebugManager.m_astar_showClosedList);
		ImGui::Checkbox("A*: Render path (green)", &gDebugManager.m_astar_showPath);

		ImGui::Separator();
		ImGui::Checkbox("Draw units visibility ranges", &gDebugManager.m_object_drawVisibilityRanges);
		ImGui::Checkbox("Allow selection of non allied units", &gDebugManager.m_object_allowNonAlliedSelection);
		ImGui::Checkbox("Allow unit production even if there is not enough food", &gDebugManager.m_ignoreFood);


		ImGui::Checkbox("Corpse vanishing", &gDebugManager.m_object_vanishing); // Careful: Objects are still associated with tile when not vanishing


		if (ImGui::Button("Close"))
			show_another_window = false;
		ImGui::End();
	}


	void DebugWindow::setCurrentTile(const Tile* pTile) {
		m_pTile = pTile;
	}
}
