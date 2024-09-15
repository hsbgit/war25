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
#include "Sound.h"
#include <stdexcept>


Sound::Sound(const std::string& strFileName) {
    m_pSample = Mix_LoadWAV(strFileName.c_str());
    if (!m_pSample) {
        // ToDo: SDL_error
        throw std::logic_error("Sound file does not exist: " + strFileName);
    }
}


Sound::~Sound() {
    //Mix_FreeChunk(sample); // ToDo: dangerous -> see emplace in map of SoundManager
}