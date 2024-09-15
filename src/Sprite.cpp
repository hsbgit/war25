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
#include "Sprite.h"
#include "Texture.h"
#include <cassert>


Sprite::Sprite() {

}


// size is an optional parameter. If not specified, the size of the complete texture is retreived and used.
Sprite::Sprite(Texture** pTexture, const Rect& size, bool isFlipped) : m_pTexture(pTexture), m_isFlipped(isFlipped), m_size(size)  {
	if (size.w == 0 || size.h == 0) {
		SDL_QueryTexture(getTexture()->getSDLTexture(), nullptr, nullptr, &m_size.w, &m_size.h);
	}
}


/*
 * Buildings / Units
 * m_size x and y (initial draw position) are typically 0 -> draw the complete sprite
 * m_offsets can be used to adjust the position where to draw the sprite.
 */
Sprite::Sprite(Texture** pTexture, const Rect& size, const Point& offsets, bool isFlipped) : m_pTexture(pTexture), m_isFlipped(isFlipped), m_size(size), m_offsets(offsets) {
	assert(m_pTexture);
}


void Sprite::updateTexture(Texture** pTexture) {
	assert(m_pTexture);
    m_pTexture = pTexture;
}
