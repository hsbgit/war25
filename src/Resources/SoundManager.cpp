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
#include <iostream>
#include <stdexcept>

SoundManager* g_pSoundManager = nullptr;

const std::string SoundManager::strFolderPrefix = "data/sounds/";



SoundManager::SoundManager() {
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
        throw std::logic_error("Error initializing SDL_mixer"); // ToDo: SDL_GetError
    }

    Mix_AllocateChannels(12); // ToDo: 12 Channels might not be available on each PC. Determine this value automatically
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

        if (!gViewport.isTileVisible(pTile)) {
            reduceVolume(channel);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl; // Todo: log info that no sufficient channels were available
    }
}


void SoundManager::playWAV(const Object* pObj, const std::string& strFileName) {
    try {
        int channel = playWAVInternal(strFileName);

        if (!gViewport.isObjectVisible(pObj)) {
            reduceVolume(channel);
        }
    }
    catch (std::exception& e) {
        // Todo: log info that no sufficient channels were available
        std::cout << "playWAV exception: " << e.what() << std::endl;
    }
}


void SoundManager::playWAV(const MapObject* pObj, const std::string& strFileName) {
    try {
        int channel = playWAVInternal(strFileName);

        if (!gViewport.isObjectVisible(pObj)) {
            reduceVolume(channel);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl; // Todo: log info that no sufficient channels were available
    }
}


// Plays a sound at full volume and returns the corresponding channel 
void SoundManager::playWAV(const std::string& strFileName) {
    try {
        playWAVInternal(strFileName);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl; // Todo: log info that no sufficient channels were available
    }
}


int SoundManager::playWAVInternal(const std::string& strFileName) {
    if (!m_sounds.contains(strFileName)) {
        std::string strPath = strFolderPrefix + strFileName;
        m_sounds.emplace(strFileName, Sound(strPath));
        std::cerr << "LOADING SOUND: " << strPath << std::endl;
    }

    const Sound& s = m_sounds.at(strFileName); // ensure that strFileName is unique

    int channel = Mix_PlayChannel(-1, s.getSample(), 0);
    if (channel == -1) {
        std::cerr << "Sound success: " << channel << "\t" << SDL_GetError() << std::endl;
        throw std::runtime_error("Mix_PlayChannel error");
    }

    Mix_Volume(channel, MIX_MAX_VOLUME);
    return channel;
}


void SoundManager::reduceVolume(int channel) {
    int volume = MIX_MAX_VOLUME;
    volume /= 8; // 50% Valume

    Mix_Volume(channel, volume);
}
