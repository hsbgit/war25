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

#include <algorithm>
#include <cstdlib>
#include <stdexcept>

Renderer* g_pRenderer = nullptr; 


Renderer::Renderer(SDL_Window* pWindow) {
    m_pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!m_pRenderer)
		m_pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_SOFTWARE);
	if (!m_pRenderer) {
		throw std::runtime_error(std::string("Error creating SDL renderer: ") + SDL_GetError());
	}
}


Renderer::~Renderer() {
    SDL_DestroyRenderer(m_pRenderer);
}

void Renderer::present() {
    // Optional one-shot frame capture for automated/headless visual checks.
    // Example: WAR25_SCREENSHOT=/tmp/frame.bmp WAR25_SCREENSHOT_FRAME=30 ./war25
    static bool screenshotCaptured = false;
    static unsigned frameNumber = 0;
    ++frameNumber;
    if (!screenshotCaptured) {
        if (const char* path = std::getenv("WAR25_SCREENSHOT")) {
            unsigned captureFrame = 1;
            if (const char* requestedFrame = std::getenv("WAR25_SCREENSHOT_FRAME"))
                captureFrame = static_cast<unsigned>(std::max(1, std::atoi(requestedFrame)));
            if (frameNumber < captureFrame) {
                SDL_RenderPresent(m_pRenderer);
                return;
            }
            int width = 0;
            int height = 0;
            if (SDL_GetRendererOutputSize(m_pRenderer, &width, &height) == 0) {
                SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ARGB8888);
                if (surface) {
                    if (SDL_RenderReadPixels(m_pRenderer, nullptr, surface->format->format,
                            surface->pixels, surface->pitch) == 0)
                        SDL_SaveBMP(surface, path);
                    SDL_FreeSurface(surface);
                }
            }
            screenshotCaptured = true;
        }
    }
    SDL_RenderPresent(m_pRenderer);
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

void Renderer::renderTintedSprite(const SDL_Point& pos, const Sprite& sprite,
                                  Uint8 r, Uint8 g, Uint8 b, Uint8 alpha) {
    SDL_Texture* texture = sprite.getTexture()->getSDLTexture();
    Uint8 oldR = 255, oldG = 255, oldB = 255, oldAlpha = 255;
    SDL_BlendMode oldBlend = SDL_BLENDMODE_NONE;
    SDL_GetTextureColorMod(texture, &oldR, &oldG, &oldB);
    SDL_GetTextureAlphaMod(texture, &oldAlpha);
    SDL_GetTextureBlendMode(texture, &oldBlend);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(texture, r, g, b);
    SDL_SetTextureAlphaMod(texture, alpha);
    renderSprite(pos, sprite);
    SDL_SetTextureColorMod(texture, oldR, oldG, oldB);
    SDL_SetTextureAlphaMod(texture, oldAlpha);
    SDL_SetTextureBlendMode(texture, oldBlend);
}

void Renderer::drawFogQuad(const Rect& rect, const Uint8 cornerAlpha[4]) {
    SDL_Vertex vertices[4] = {
        {{static_cast<float>(rect.x), static_cast<float>(rect.y)}, {0, 0, 0, cornerAlpha[0]}, {0, 0}},
        {{static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y)}, {0, 0, 0, cornerAlpha[1]}, {0, 0}},
        {{static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y + rect.h)}, {0, 0, 0, cornerAlpha[2]}, {0, 0}},
        {{static_cast<float>(rect.x), static_cast<float>(rect.y + rect.h)}, {0, 0, 0, cornerAlpha[3]}, {0, 0}}
    };
    const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(m_pRenderer, nullptr, vertices, 4, indices, 6);
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
