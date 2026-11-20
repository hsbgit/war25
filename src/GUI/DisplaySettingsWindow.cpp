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
#include "Renderer.h"
#include <algorithm>
#include <set>
#include <string>

namespace gui {
	DisplaySettingsWindow::DisplaySettingsWindow(SDL_Window* window) : m_window(window) {
		loadAvailableResolutions();
		findCurrentResolutionIndex();

		Uint32 flags = SDL_GetWindowFlags(m_window);
		m_fullscreenEnabled = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;

		SDL_RendererInfo rendererInfo{};
		if (g_pRenderer && SDL_GetRendererInfo(g_pRenderer->accessSDLRenderer(), &rendererInfo) == 0)
			m_vsyncEnabled = (rendererInfo.flags & SDL_RENDERER_PRESENTVSYNC) != 0;

		m_appliedResolutionIndex = m_selectedResolutionIndex;
		m_appliedFullscreen = m_fullscreenEnabled;
		m_appliedVSync = m_vsyncEnabled;

		m_visible = true;
	}

	void DisplaySettingsWindow::draw() {
		if (!m_visible)
			return;


		ImGui::SetNextWindowSize(ImVec2(420, 270), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(
			std::max(10.0f, ImGui::GetIO().DisplaySize.x - 650.0f), 30.0f), ImGuiCond_Once);

		const bool drawContents = ImGui::Begin("Display Settings", &m_visible,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if (!m_visible) {
			discardPendingChanges();
			ImGui::End();
			return;
		}
		if (!drawContents) {
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 7));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 10));

		ImGui::TextColored(ImVec4(0.35f, 0.72f, 1.0f, 1.0f), "Window");
		ImGui::Separator();
		ImGui::TextUnformatted("Resolution");
		ImGui::BeginDisabled(m_fullscreenEnabled);
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
		ImGui::EndDisabled();
		if (m_fullscreenEnabled)
			ImGui::TextDisabled("Desktop fullscreen uses the monitor's native resolution.");

		ImGui::Checkbox("Desktop fullscreen", &m_fullscreenEnabled);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Borderless fullscreen at the current desktop resolution.");

		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.35f, 0.72f, 1.0f, 1.0f), "Rendering");
		ImGui::Separator();
		ImGui::Checkbox("Vertical synchronization (VSync)", &m_vsyncEnabled);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Synchronizes presentation with the monitor to reduce tearing.");

		const ImVec2 buttonSize(112, 32);
		const float buttonsWidth = buttonSize.x * 2.0f + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - buttonSize.y - ImGui::GetStyle().WindowPadding.y);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonsWidth - ImGui::GetStyle().WindowPadding.x);
		if (ImGui::Button("Cancel", buttonSize)) {
			discardPendingChanges();
			m_visible = false;
		}
		ImGui::SameLine();
		const bool pending = hasPendingChanges();
		ImGui::BeginDisabled(!pending);
		if (ImGui::Button("Apply", buttonSize)) {
			m_applySettings = true;
		}
		ImGui::EndDisabled();

		ImGui::PopStyleVar(2);
		ImGui::End();
	}

	bool DisplaySettingsWindow::hasPendingChanges() const {
		return m_selectedResolutionIndex != m_appliedResolutionIndex ||
			m_fullscreenEnabled != m_appliedFullscreen || m_vsyncEnabled != m_appliedVSync;
	}

	void DisplaySettingsWindow::discardPendingChanges() {
		m_selectedResolutionIndex = m_appliedResolutionIndex;
		m_fullscreenEnabled = m_appliedFullscreen;
		m_vsyncEnabled = m_appliedVSync;
		m_applySettings = false;
	}

	void DisplaySettingsWindow::resetApplyFlag() {
		m_applySettings = false;
		m_appliedResolutionIndex = m_selectedResolutionIndex;
		m_appliedFullscreen = m_fullscreenEnabled;
		m_appliedVSync = m_vsyncEnabled;
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
