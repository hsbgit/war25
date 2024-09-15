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

#include <vector>
#include <cassert>

class Sprite;
class Texture;

#include "Types.h"


class Animation {
public:   
    Animation();
    Animation(Texture* pBaseTexture, unsigned int interval_ms = 0, bool playOnce = false);

    // Different units may (of course) use the same animation at different timepoints
    // Hence, the ResourceManager returns copies of existing animations.
    Animation(const Animation& a) {
        operator=(a);
    }

    Animation& operator=(const Animation& a);

    inline const Sprite& getCurrentFrame() const {
        return m_vecSprites[m_currentSpriteID];
    }


    void setBaseTexture(Texture* pBaseTexture);
    void setOffset(const Point& offset);


    inline Texture** getBaseTexture() {
        return &m_pBaseTexture;
    }

    void addSprite(const Sprite& sprite);
    void process();


    // If an animation is of type playOnce this method checks whether all frames had been shown
    inline bool isComplete() const {
        assert(m_playOnce);
        return m_animationFinished;
    }

private:
    void nextSprite();

    int m_ticksPerSprite = -1;

    unsigned int m_passedTime_ms = 0, m_interval_ms = 0;
    bool m_playOnce = true, m_animationFinished = false;

    Texture* m_pBaseTexture = nullptr;
    std::vector<Sprite> m_vecSprites;
    std::size_t m_currentSpriteID = 0;
};
