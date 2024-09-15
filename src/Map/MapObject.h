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

#include "Global/Object.h"
#include "Sprite.h"


class MapObject {
public:
    MapObject(Object* pSource) : m_pSource(pSource) {

    }

    virtual ~MapObject() {}

    virtual void process() = 0;
    virtual bool isExpired() const = 0;

    virtual Sprite& getSprite() = 0;

    virtual const Point& getPositionWorld_px() const = 0;


protected:
    Object* m_pSource;
};