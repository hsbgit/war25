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
#include "State.h"
#include <cassert>


State::State(Object* pOwner) : m_pOwner(pOwner) {
    assert(m_pOwner);
}


State* State::transitionToNextState() {
    assert(m_pNextState);
    onExit();
    m_pNextState->onEnter();
    return m_pNextState;
}