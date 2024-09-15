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
#include "PUDBrowser.h"
#include "imgui/imgui.h"

#include <string>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;


namespace gui {
    const std::string strPrefix = "data/maps/";


    PUDBrowser::PUDBrowser() : ImGuiWindow() {
        readDirectoryRecursively(strPrefix);

        // Soundmanager must be ready at this point! 
        const std::string strMusicFolderPath = "data/maps/";

        if (std::filesystem::exists(strMusicFolderPath)) {
            for (const auto& entry : fs::directory_iterator(strMusicFolderPath)) {
                if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".pud") {
               //     m_vecMidiFiles.push_back(strMusicFolderPath + entry.path().filename().string());
                }
            }
        }
    }


    const std::string& PUDBrowser::selectedMap() const {
        return m_strSelectedMap;
    }


    void PUDBrowser::readDirectoryRecursively(const std::string& pathToMapsFolder) {
        for (const auto& entry : fs::directory_iterator(pathToMapsFolder)) {
        //    std::cout << entry.path() << " " << entry.is_directory() << " " << entry.is_regular_file() << " " << fs::path(entry.path()).extension() << std::endl;
          //  std::cout << "\t\t: " << entry.path().string().erase(0, strPrefix.size()) << std::endl;

            if (entry.is_directory()) {
                readDirectoryRecursively(entry.path().string());
            }
            else if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".pud") {
             //    vecMaps.push_back(entry.path().filename().string());  // Complete relative path
                 vecMaps.push_back(entry.path().string()); // Only file name
            }
        }
    }


    void PUDBrowser::draw() {
        ImGui::Begin("PUDBrowser");

        ImGui::InputText("Enter Text to Filter Maps", m_searchText, sizeof(m_searchText));

        int numVisibleItems = 30;

        if (ImGui::BeginListBox("PUD Browser Listbox", ImVec2(-FLT_MIN, numVisibleItems * ImGui::GetTextLineHeightWithSpacing()))) {
            for (std::size_t i = 0; i < vecMaps.size(); ++i) {
                std::string mapName = vecMaps[i];

                // If searchText is not empty and mapName does not contain searchText, continue to next iteration
                if (strlen(m_searchText) > 0 && mapName.find(m_searchText) == std::string::npos) {
                    continue;
                }


                const bool is_selected = (m_current_index == i);
                if (ImGui::Selectable(mapName.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick)) {
                    m_current_index = (int)i;

                    if (ImGui::IsMouseDoubleClicked(0)) {
                  
                        m_strSelectedMap = vecMaps[i];

                        if (ImGui::BeginPopupModal("Load Image?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                        {
                            ImGui::Text("This line is a regular text line and works fine");
                            ImGui::TextWrapped("This is a wrapped line and will cause a wrong Y centering");

                            if (ImGui::Button("OK", ImVec2(120, 40))) { ImGui::CloseCurrentPopup(); }
                            ImGui::SameLine();
                            if (ImGui::Button("Cancel", ImVec2(120, 40))) { ImGui::CloseCurrentPopup(); }
                            ImGui::EndPopup();
                        }


                    }
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }

        ImGui::Separator();       

        ImGui::End();
    }
}
