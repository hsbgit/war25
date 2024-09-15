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
#include "MusicPlayer.h"
#include "imgui/imgui.h"
#include "Global/PseudoRandom.h"
#include <filesystem>


namespace gui {

	bool MusicPlayer::m_musicFinished = false;


	MusicPlayer::MusicPlayer() : ImGuiWindow() {
		namespace fs = std::filesystem;

		// Soundmanager muss bereits laufen! Dort ist SDL_Mixer init
		const std::string strMusicFolderPath = "data/music/";

		if (std::filesystem::exists(strMusicFolderPath)) {
			for (const auto& entry : fs::directory_iterator(strMusicFolderPath)) {
				if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".mid") {
					m_vecMidiFiles.push_back(strMusicFolderPath + entry.path().filename().string());
				}
			}
		}

		Mix_HookMusicFinished(onMusicFinished);
	}


	MusicPlayer::~MusicPlayer() {
		Mix_FreeMusic(m_pMusic);
	}


	void MusicPlayer::onMusicFinished() {
		m_musicFinished = true;
	}


	void MusicPlayer::playMusic() {
		m_pMusic = Mix_LoadMUS(m_vecMidiFiles[m_current_index].c_str());
		if (!m_pMusic) {
			throw std::logic_error("Could not open music file!" + std::string(SDL_GetError()));
		}
		Mix_PlayMusic(m_pMusic, 0);
	}


	void MusicPlayer::draw() {
		ImGui::Begin("Music Player");

		if (ImGui::BeginListBox("Music Player Listbox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
			for (std::size_t i = 0; i < m_vecMidiFiles.size(); ++i) {
				const bool is_selected = (m_current_index == i);
				if (ImGui::Selectable(m_vecMidiFiles[i].c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick)) {
					m_current_index = (int)i;

					if (ImGui::IsMouseDoubleClicked(0)) {
						playMusic();
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

		if (ImGui::Button("    <<    ")) {
			if (--m_current_index < 0) {
				m_current_index = (int)m_vecMidiFiles.size() - 1;
			}

			playMusic();
		}

		ImGui::SameLine();
		if (ImGui::Button("    >>    ")) {
			if (++m_current_index == (int)m_vecMidiFiles.size()) {
				m_current_index = 0;
			}

			playMusic();
		}

		ImGui::SameLine();
		if (ImGui::Button(" Stop ")) {
			Mix_FreeMusic(m_pMusic);
			m_pMusic = nullptr;
		}

		ImGui::SameLine();
		if (ImGui::Button(" Play ")) {
			playMusic();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Shuffle", &m_shuffleActive);

		if (m_musicFinished) {
			m_musicFinished = false;

			if (m_shuffleActive) {
				m_current_index = PseudoRandom::rnd(0, (int)m_vecMidiFiles.size() - 1);
			}
			else if (++m_current_index == m_vecMidiFiles.size()) {
				m_current_index = 0;
			}

			playMusic();
		}

		ImGui::End();
	}
}