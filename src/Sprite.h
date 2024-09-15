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

#include "Types.h"
#include <string>
#include <cassert>

class Texture;

class Sprite {
public:
    Sprite();
    Sprite(Texture** pTexture, const Rect& size = { 0,0,0,0 }, bool isFlipped = false);
    Sprite(Texture** pTexture, const Rect& size, const Point& offsets, bool isFlipped = false);


    void updateTexture(Texture** pTexture);

    inline void setOffset(const Point& offset) {
        m_offsets = offset;
    }


    inline int getOffsetPosX() const {
        return m_offsets.x;
    }

    inline int getOffsetPosY() const {
        return m_offsets.y;
    }

    inline int getOffsetTextureX() const {
        return m_size.x;
    }

    inline int getOffsetTextureY() const {
        return m_size.y;
    }

    inline int getWidth() const {
        return m_size.w;
    }

    inline int getHeight() const {
        return m_size.h;
    }

    inline Texture* getTexture() const {
        assert(m_pTexture);
        return *m_pTexture;
    }

    inline bool isFlipped() const {
        return m_isFlipped;
    }

    inline bool isValid() const {
        return (m_pTexture);
    }

private:
    /*
     * Using a pointer to a pointer allows switching the texture of a sprite during runtime.
     * Example: Tileset is summer, can be switched to winter at the ResourceManager_wartools and all sprites stay valid.
     */
    Texture** m_pTexture = nullptr;
    bool m_isFlipped = false;
    Rect m_size = { 0, 0, 0, 0 };
    Point m_offsets = { 0, 0 };
};
