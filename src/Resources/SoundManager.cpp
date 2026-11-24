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
#include "SoundManager.h"
#include "AudioRules.h"
#include <iostream>
#include <stdexcept>

SoundManager* g_pSoundManager = nullptr;

const std::string SoundManager::strFolderPrefix = "data/sounds/";



SoundManager::SoundManager() {
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
		throw std::runtime_error(std::string("Error initializing SDL_mixer: ") + Mix_GetError());
    }

	Mix_AllocateChannels(32);
	Mix_ReserveChannels(1); // Channel 0 is dedicated to non-overlapping selection voices.
}


SoundManager::~SoundManager() {
    Mix_CloseAudio();
}


void SoundManager::playErrorWAV() {
    playWAV("ui/placement_error.wav");
}


void SoundManager::playWAV(const Tile* pTile, const std::string& strFileName) {
    try {
        int channel = playWAVInternal(strFileName);

        if (channel >= 0 && !gViewport.isTileVisible(pTile)) {
            reduceVolume(channel);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Unable to play positional sound: " << e.what() << std::endl;
    }
}


void SoundManager::playWAV(const Object* pObj, const std::string& strFileName) {
    try {
        int channel = playWAVInternal(strFileName);

        if (channel >= 0 && !gViewport.isObjectVisible(pObj)) {
            reduceVolume(channel);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Unable to play object sound: " << e.what() << std::endl;
    }
}


void SoundManager::playWAV(const MapObject* pObj, const std::string& strFileName) {
    try {
        int channel = playWAVInternal(strFileName);

        if (channel >= 0 && !gViewport.isObjectVisible(pObj)) {
            reduceVolume(channel);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Unable to play map-object sound: " << e.what() << std::endl;
    }
}


// Plays a sound at full volume and returns the corresponding channel 
void SoundManager::playWAV(const std::string& strFileName) {
    try {
        playWAVInternal(strFileName);
    }
    catch (std::exception& e) {
        std::cerr << "Unable to play sound: " << e.what() << std::endl;
    }
}

void SoundManager::playSelectionWAV(const std::string& strFileName) {
	try {
		playSelectionWAVInternal(strFileName);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void SoundManager::playSelectionWAV(const Object* pObj, const std::string& strFileName) {
	try {
		const int channel = playSelectionWAVInternal(strFileName);
		if (channel >= 0 && !gViewport.isObjectVisible(pObj))
			reduceVolume(channel);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

int SoundManager::playSelectionWAVInternal(const std::string& strFileName) {
	if (Mix_Playing(0))
		return -1;
	if (!m_sounds.contains(strFileName)) {
		const std::string path = strFolderPrefix + strFileName;
		m_sounds.emplace(strFileName, Sound(path));
		std::cerr << "LOADING SOUND: " << path << std::endl;
	}
	const int channel = Mix_PlayChannel(0, m_sounds.at(strFileName).getSample(), 0);
	if (channel == -1)
		throw std::runtime_error("Mix_PlayChannel error");
	Mix_Volume(channel, MIX_MAX_VOLUME);
	return channel;
}


int SoundManager::playWAVInternal(const std::string& strFileName) {
	if (!AudioRules::registerGameplaySound(
		m_gameplaySoundsPlayedThisFrame, strFileName))
		return -1;
    if (!m_sounds.contains(strFileName)) {
        std::string strPath = strFolderPrefix + strFileName;
        m_sounds.emplace(strFileName, Sound(strPath));
        std::cerr << "LOADING SOUND: " << strPath << std::endl;
    }

    const Sound& s = m_sounds.at(strFileName); // ensure that strFileName is unique

    int channel = Mix_PlayChannel(-1, s.getSample(), 0);
    if (channel == -1) {
		return -1;
    }

    Mix_Volume(channel, MIX_MAX_VOLUME);
    return channel;
}

void SoundManager::beginGameFrame() {
	m_gameplaySoundsPlayedThisFrame.clear();
}


void SoundManager::reduceVolume(int channel) {
    int volume = MIX_MAX_VOLUME;
    volume /= 8; // 50% Valume

    Mix_Volume(channel, volume);
}
