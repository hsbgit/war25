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
#include "SoundCache.h"
#include <filesystem>

namespace War25::Audio {
    void SoundCache::preloadSound(const std::string& soundPath) {
        if (std::filesystem::exists(soundPath)) {
            m_soundCache[soundPath] = CachedSound{
                soundPath,
                std::chrono::steady_clock::time_point{},
                false
            };
        }
    }

    void SoundCache::preloadSounds(const std::vector<std::string>& soundPaths) {
        for (const auto& path : soundPaths) {
            preloadSound(path);
        }
    }

    bool SoundCache::isSoundCached(const std::string& soundPath) const {
        return m_soundCache.find(soundPath) != m_soundCache.end();
    }

    void SoundCache::clearCache() {
        m_soundCache.clear();
    }

    bool SoundCache::canPlaySound(const std::string& soundPath,
                                 std::chrono::milliseconds minInterval) const {
        auto it = m_soundCache.find(soundPath);
        if (it == m_soundCache.end()) {
            return true; // Not in cache, can play
        }

        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastPlay = now - it->second.lastPlayed;
        return timeSinceLastPlay >= minInterval;
    }

    void SoundCache::markSoundPlayed(const std::string& soundPath) {
        auto& sound = m_soundCache[soundPath];
        sound.lastPlayed = std::chrono::steady_clock::now();
    }
}