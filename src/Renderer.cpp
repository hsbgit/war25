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
#include "Renderer.h"

Renderer* g_pRenderer = nullptr; 


Renderer::Renderer(SDL_Window* pWindow) {
    m_pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (m_pRenderer == 0){
        // ToDo: throw
        SDL_Log("Error creating SDL_Renderer!");
    }
}


Renderer::~Renderer() {
    SDL_DestroyRenderer(m_pRenderer);
}


void Renderer::drawRect(const Rect& rect, int r, int g, int b) {
    SDL_SetRenderDrawColor(m_pRenderer, r, g, b, 255);
    SDL_RenderDrawRect(m_pRenderer, &rect);
}


void Renderer::drawFilledRect(const Rect &rect, int r, int g, int b, int a)  {
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_pRenderer, r, g, b, a);
    SDL_RenderFillRect(m_pRenderer, &rect);
}


void Renderer::renderTexture(const Rect& r, SDL_Texture* pTexture) {
    SDL_RenderCopy(m_pRenderer, pTexture, nullptr, &r);
}


void Renderer::renderTexture(const Point &pos, const Texture *t) {
    Rect rectAnim;
    rectAnim.x = pos.x;
    rectAnim.y = pos.y;
    rectAnim.w = t->getWidth();
    rectAnim.h = t->getHeight();
    SDL_RenderCopy(m_pRenderer, t->getSDLTexture(), nullptr, &rectAnim);
}

