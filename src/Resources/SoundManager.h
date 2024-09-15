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


#include <SDL_mixer.h>
#include <string>
#include <stdexcept>
#include <unordered_map>

#include "Map/MapObject.h"
#include "Global/Viewport.h"
#include "Sound.h"


class SoundManager {
public:
    static const std::string strFolderPrefix;

    SoundManager();
    ~SoundManager();

    void playErrorWAV();

    void playWAV(const Tile* pTile, const std::string& strFileName);
    void playWAV(const Object* pObj, const std::string& strFileName);
    void playWAV(const MapObject* pObj, const std::string& strFileName);
    void playWAV(const std::string& strFileName);


private:
    int playWAVInternal(const std::string& strFileName);
    void reduceVolume(int channel);

    std::unordered_map<std::string, Sound> m_sounds;

};

// Pointer as this object must be initialized after the SDL_Init call
extern SoundManager* g_pSoundManager;
