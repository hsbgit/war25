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

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class Renderer;

class Texture {
public:
    Texture(SDL_Texture* pSDLTexture = nullptr, int width = -1, int height = -1);
    Texture(const std::string& pathToImg, Renderer* pRenderer, bool freeSurface = true);
    // Careful: pSurface is freed by the ctor after a Texture is created!
    Texture(SDL_Surface* pSurface, Renderer* pRenderer, bool freeSurface = true);
    ~Texture();

    inline SDL_Texture* getSDLTexture() const {
        return m_pSDLTexture;
    }

    inline int getWidth() const {
        return m_width;
    }

    inline int getHeight() const {
        return m_height;
    }

private:
    SDL_Texture* m_pSDLTexture = nullptr;
    int m_width = -1, m_height = -1;
    void convertSurfaceToTexture(SDL_Surface* pSurface, Renderer* pRenderer, bool freeSurface);
};
