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

#include "Sprite.h"
#include "Texture.h"
#include "Types.h"


class Renderer  {
public:
    Renderer(SDL_Window* pWindow);
    ~Renderer();

    void renderTexture(const SDL_Point &pos, const Texture *t) ;
    void drawRect(const SDL_Rect& rect, int r, int g, int b) ;
    void drawFilledRect(const SDL_Rect &rect, int r, int g, int b, int a) ;

    inline void clear() {
        SDL_SetRenderDrawColor(m_pRenderer, 0,0,0,0);
        SDL_RenderClear(m_pRenderer);
    }

    inline void present() {
        SDL_RenderPresent(m_pRenderer);
    }

    inline SDL_Renderer* SDLRenderer() {
        return m_pRenderer;
    }

    inline void renderSprite(const SDL_Point& pos, const Sprite& sprite) {
        renderSprite(pos, &sprite);
    }

    inline void renderSprite(const SDL_Point& pos, const Sprite* pSprite) {
        SDL_Rect src = {pSprite->getOffsetTextureX(), pSprite->getOffsetTextureY(), pSprite->getWidth(), pSprite->getHeight()}; // Rect inside the Sprite's texture (may be the complete Texture)
        SDL_Rect dst = {pos.x + pSprite->getOffsetPosX(), pos.y + pSprite->getOffsetPosY(), pSprite->getWidth(), pSprite->getHeight()}; // Where to render

        SDL_RendererFlip flip = pSprite->isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(m_pRenderer, pSprite->getTexture()->getSDLTexture(), &src, &dst, 0.0, nullptr, flip);
    }

    // Size of pSprite will be adjusted to 'rect'. 
    // Flipping property of pSprite is not evaluated.
    inline void renderSprite(const SDL_Rect& rect, Sprite* pSprite) {
        renderTexture(rect, pSprite->getTexture()->getSDLTexture());
    }

    void renderTexture(const SDL_Rect& r, SDL_Texture* pTexture);

    SDL_Renderer* accessSDLRenderer() const {
        return m_pRenderer;
    }

private:
    SDL_Renderer *m_pRenderer = 0;   
};

extern Renderer* g_pRenderer;
