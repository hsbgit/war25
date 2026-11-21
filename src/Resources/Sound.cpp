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
		throw std::runtime_error("Could not load sound '" + strFileName + "': " + Mix_GetError());
    }
}

Sound::Sound(Sound&& other) noexcept : m_pSample(other.m_pSample) {
	other.m_pSample = nullptr;
}

Sound& Sound::operator=(Sound&& other) noexcept {
	if (this == &other) return *this;
	if (m_pSample) Mix_FreeChunk(m_pSample);
	m_pSample = other.m_pSample;
	other.m_pSample = nullptr;
	return *this;
}

Sound::~Sound() {
	if (m_pSample) Mix_FreeChunk(m_pSample);
}
