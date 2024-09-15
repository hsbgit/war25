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

#include <SDL_mixer.h>
#include <string>
#include <vector>

namespace gui {
	class MusicPlayer : public ImGuiWindow {
	public:
		MusicPlayer();
		~MusicPlayer();

		void draw() override;

	private:
		void playMusic();
		static void onMusicFinished(); // C callback can only be connected to static class methods (=functions)
		static bool m_musicFinished; // Set by 'onMusicFinished', evaluated inside 'draw()'

		bool m_shuffleActive = true;
		Mix_Music* m_pMusic = nullptr;

		int m_current_index = 0;
		std::vector<std::string> m_vecMidiFiles;
	};
}