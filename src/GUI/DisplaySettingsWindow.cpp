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
#include "DisplaySettingsWindow.h"
#include "imgui/imgui.h"
#include "Global/Constants.h"
#include <algorithm>
#include <set>
#include <string>

namespace gui {
	DisplaySettingsWindow::DisplaySettingsWindow(SDL_Window* window) : m_window(window) {
		loadAvailableResolutions();
		findCurrentResolutionIndex();

		Uint32 flags = SDL_GetWindowFlags(m_window);
		m_fullscreenEnabled = (flags & SDL_WINDOW_FULLSCREEN) != 0;

		m_visible = true;
	}

	void DisplaySettingsWindow::draw() {
		if (!m_visible)
			return;


		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("🖥️ Display Settings", &m_visible,
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 12));

		ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Video Configuration");
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Resolution:");
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##ResolutionCombo",
			(std::to_string(m_availableResolutions[m_selectedResolutionIndex].width) + " x " +
			 std::to_string(m_availableResolutions[m_selectedResolutionIndex].height) + " px").c_str())) {

			for (int i = 0; i < m_availableResolutions.size(); i++) {
				const bool isSelected = (m_selectedResolutionIndex == i);
				std::string resolutionText = std::to_string(m_availableResolutions[i].width) + " x " +
											std::to_string(m_availableResolutions[i].height) + " px";

				if (ImGui::Selectable(resolutionText.c_str(), isSelected)) {
					m_selectedResolutionIndex = i;
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();
		ImGui::Checkbox("🖼️ Fullscreen Mode", &m_fullscreenEnabled);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImVec2 buttonSize = ImVec2(120, 32);
		float windowWidth = ImGui::GetWindowSize().x;
		float buttonSpacing = 16;
		float totalButtonWidth = 2 * buttonSize.x + buttonSpacing;
		float startX = (windowWidth - totalButtonWidth) * 0.5f;

		ImGui::SetCursorPosX(startX);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
		if (ImGui::Button("✓ Apply", buttonSize)) {
			m_applySettings = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("✗ Close", buttonSize)) {
			m_visible = false;
		}
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar(2);
		ImGui::End();
	}

	void DisplaySettingsWindow::loadAvailableResolutions() {
		m_availableResolutions.clear();

		int displayIndex = SDL_GetWindowDisplayIndex(m_window);
		if (displayIndex < 0) {
			displayIndex = 0;
		}

		int numModes = SDL_GetNumDisplayModes(displayIndex);
		std::set<std::pair<int, int>> uniqueResolutions;

		for (int i = 0; i < numModes; i++) {
			SDL_DisplayMode mode;
			if (SDL_GetDisplayMode(displayIndex, i, &mode) == 0) {
				if (mode.w >= War25::Constants::MIN_WINDOW_WIDTH &&
				    mode.h >= War25::Constants::MIN_WINDOW_HEIGHT) {
					uniqueResolutions.insert({mode.w, mode.h});
				}
			}
		}

		for (const auto& res : uniqueResolutions) {
			m_availableResolutions.push_back({res.first, res.second});
		}

		std::sort(m_availableResolutions.begin(), m_availableResolutions.end(),
			[](const Resolution& a, const Resolution& b) {
				if (a.width != b.width) return a.width < b.width;
				return a.height < b.height;
			});

		bool has1600x1280 = false;
		for (const auto& res : m_availableResolutions) {
			if (res.width == 1600 && res.height == 1280) {
				has1600x1280 = true;
				break;
			}
		}
		if (!has1600x1280) {
			m_availableResolutions.push_back({1600, 1280});
			std::sort(m_availableResolutions.begin(), m_availableResolutions.end(),
				[](const Resolution& a, const Resolution& b) {
					if (a.width != b.width) return a.width < b.width;
					return a.height < b.height;
				});
		}

		if (m_availableResolutions.empty()) {
			using namespace War25::Constants;
			m_availableResolutions.push_back({MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT});
			m_availableResolutions.push_back({1024, 768});
			m_availableResolutions.push_back({1280, 720});
			m_availableResolutions.push_back({DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT});
			m_availableResolutions.push_back({1920, 1080});
		}
	}

	void DisplaySettingsWindow::findCurrentResolutionIndex() {
		int currentWidth, currentHeight;
		SDL_GetWindowSize(m_window, &currentWidth, &currentHeight);

		Resolution currentRes = {currentWidth, currentHeight};

		for (int i = 0; i < m_availableResolutions.size(); i++) {
			if (m_availableResolutions[i] == currentRes) {
				m_selectedResolutionIndex = i;
				return;
			}
		}

		Resolution targetRes = {1600, 1280};
		for (int i = 0; i < m_availableResolutions.size(); i++) {
			if (m_availableResolutions[i] == targetRes) {
				m_selectedResolutionIndex = i;
				return;
			}
		}

		m_selectedResolutionIndex = 0;
	}
}