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
#include "Animation.h"
#include "Global/GlobalAttributes.h"
#include "Sprite.h"


Animation::Animation() {

}


Animation::Animation(Texture *pBaseTexture, unsigned int interval_ms, bool playOnce) : m_interval_ms(interval_ms), m_playOnce(playOnce), m_pBaseTexture(pBaseTexture), m_currentSpriteID(0) {
    assert(m_pBaseTexture);
    m_ticksPerSprite = calculateRequiredTicksForGameAction(interval_ms);
}


Animation& Animation::operator=(const Animation& a) {
    if(this != &a) {        
        m_pBaseTexture = a.m_pBaseTexture;

        m_vecSprites = a.m_vecSprites;
        /*
         * All sprites refer to an animation's local m_pBaseTexture pointer.
         * If we use the compiler generated copy constructor and assignment operator,
         * the sprites of the our animation object (=this) still refer to a.m_pBaseTexture.
         *
         * This would be fine but if the other animation object gets destroyed we have dangling pointers.
         * This means, we update the sprite references to our new animation object.
         */
        for(Sprite& sprite : m_vecSprites) {
            sprite.updateTexture(&m_pBaseTexture);
        }

        m_currentSpriteID = a.m_currentSpriteID;
        m_passedTime_ms = a.m_passedTime_ms;
        m_interval_ms = a.m_interval_ms;
        m_playOnce = a.m_playOnce;
        m_animationFinished = false;

        m_ticksPerSprite = a.m_ticksPerSprite;
    }

    return *this;
}


/*
 * Sets the underlying texture all animation sprites refer to.
 * This allows to switch the PlayerColor easily, e.g., from blue to green
 * without the need to create the same animations for each color.
 */
void Animation::setBaseTexture(Texture* pBaseTexture) {
    assert(pBaseTexture);
    m_pBaseTexture = pBaseTexture;
}


void Animation::setOffset(const Point& offset) {
    for (Sprite& sprite : m_vecSprites) {
        sprite.setOffset(offset);
    }
}


void Animation::addSprite(const Sprite& sprite) {    
    m_vecSprites.push_back(sprite);
}


void Animation::process() {
    if(m_vecSprites.size() == 1|| (m_playOnce && m_animationFinished)) {
        return;
    }

    --m_ticksPerSprite;

   if(m_ticksPerSprite == 0) {
       nextSprite();
       m_ticksPerSprite = calculateRequiredTicksForGameAction(m_interval_ms);
   }
}


void Animation::nextSprite() {
    if(++m_currentSpriteID >= m_vecSprites.size()) {
        if(m_playOnce)
            m_animationFinished = true;
        else 
            m_currentSpriteID = 0;        
    }
}
