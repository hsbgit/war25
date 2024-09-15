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

#include "MenuBarWindow.h"
#include <sstream>
#include "Player.h"


namespace gui {
	void MenuBarWindow::draw() {
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Main"))
			{
				if (ImGui::MenuItem("Quit")) {


				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Debug")) {
				if (ImGui::MenuItem("Magic")) {}
				if (ImGui::MenuItem("Information")) {}
				if (ImGui::MenuItem("ImGui Demo Window")) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Player Statistics")) {}
				ImGui::EndMenu();
			}


			// Todo: do it the right way
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			// Todo: icons für ressourcen


			bool enoughFood = (m_pPlayer->getAvailableFood() >= m_pPlayer->getUsedFood());
			std::string strFoodInfo("Food: " + std::to_string(m_pPlayer->getUsedFood()) + "/" + std::to_string(m_pPlayer->getAvailableFood()));

			if (enoughFood) {
				ImGui::Text("%s", strFoodInfo.c_str());
			}
			else {
				static ImVec4 colorRed = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
				ImGui::TextColored(colorRed, "%s", strFoodInfo.c_str());
			}

			Resources resAvail = m_pPlayer->availableResources();
			Resources resIncreased = m_pPlayer->getAdditionResourceProduction();

			std::stringstream ssInfoGold;
			ssInfoGold << "Gold: " << resAvail.gold() << " (+" << resIncreased.gold() << " extra)";
			static ImVec4 colorGold = ImVec4(1.0f, 0.84f, 0.0f, 1.00f);
			ImGui::TextColored(colorGold, "%s", ssInfoGold.str().c_str());


			std::stringstream ssInfoLumber;
			ssInfoLumber << "Lumber: " << resAvail.lumber() << " (+" << resIncreased.lumber() << " extra)";
			static ImVec4 colorLumber = ImVec4(0.23f, 0.74f, 0.23f, 1.00f);
			ImGui::SameLine();
			ImGui::TextColored(colorLumber, "%s", ssInfoLumber.str().c_str());

			std::stringstream ssInfoOil;
			ssInfoOil << "Oil: " << resAvail.oil() << " (+" << resIncreased.oil() << " extra)";
			static ImVec4 colorOil = ImVec4(0.6f, 0.62f, 0.66f, 1.00f);
			ImGui::SameLine();
			ImGui::TextColored(colorOil, "%s", ssInfoOil.str().c_str());

			ImGui::EndMainMenuBar();
		}
	}
}