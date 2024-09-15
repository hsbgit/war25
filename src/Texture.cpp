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
#include "Texture.h"
#include "Renderer.h"
#include <iostream>
#include <cassert>


Texture::Texture(SDL_Texture* pSDLTexture, int width, int height) : m_pSDLTexture(pSDLTexture), m_width(width), m_height(height) {

}



Texture::Texture(const std::string& pathToImg, Renderer* pRenderer, bool freeSurface) {
    SDL_Surface* pSurface  = IMG_Load(pathToImg.c_str());

    if(!pSurface) {
        std::cout << "Coult not load file " << pathToImg << ", " << SDL_GetError() << std::endl;
        assert(0);
        // ToDo: exception;
    }  

    convertSurfaceToTexture(pSurface, pRenderer, freeSurface);
}


Texture::~Texture() {
    SDL_DestroyTexture(m_pSDLTexture);
}


// Careful: pSurface is freed by the ctor after a Texture is created!
Texture::Texture(SDL_Surface* pSurface, Renderer* pRenderer, bool freeSurface) {
    convertSurfaceToTexture(pSurface, pRenderer, freeSurface);
}


void Texture::convertSurfaceToTexture(SDL_Surface* pSurface, Renderer* pRenderer, bool freeSurface) {
    m_pSDLTexture = SDL_CreateTextureFromSurface(pRenderer->SDLRenderer(), pSurface);

    if(!m_pSDLTexture) {
        std::cerr << "Coult not create texture from surface: " << SDL_GetError() << std::endl;
        assert(0);
        // ToDo: exception;
    }

    if(freeSurface)
        SDL_FreeSurface(pSurface);

    SDL_QueryTexture(m_pSDLTexture, nullptr, nullptr, &m_width, &m_height);
}
