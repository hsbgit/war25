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

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>

namespace War25::Audio {
    class SoundCache {
    public:
        struct CachedSound {
            std::string filepath;
            std::chrono::steady_clock::time_point lastPlayed;
            bool isLooping = false;
        };

        static SoundCache& getInstance() {
            static SoundCache instance;
            return instance;
        }

        void preloadSound(const std::string& soundPath);
        void preloadSounds(const std::vector<std::string>& soundPaths);
        bool isSoundCached(const std::string& soundPath) const;
        void clearCache();

        // Prevent duplicate sounds playing too quickly
        bool canPlaySound(const std::string& soundPath,
                         std::chrono::milliseconds minInterval = std::chrono::milliseconds(100)) const;
        void markSoundPlayed(const std::string& soundPath);

    private:
        SoundCache() = default;
        ~SoundCache() = default;
        SoundCache(const SoundCache&) = delete;
        SoundCache& operator=(const SoundCache&) = delete;

        std::unordered_map<std::string, CachedSound> m_soundCache;
    };
}