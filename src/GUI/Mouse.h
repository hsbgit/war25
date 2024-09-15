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

#include <SDL2/SDL.h>
#include <functional>


class Mouse {
public:
	enum class Button { Left, Right };
	bool consume(SDL_Event& event);

	// funktion "mous im bild"?


	inline bool pressed() const {
		return m_mousePressed;
	}

	// Information only valid if 'pressed()' returned true
	inline bool isLeftButtonPressed() const {
		return m_leftButtonPressed;
	}

	// Information only valid if 'pressed()' returned true
	inline bool isRightButtonPressed() const {
		return m_rightButtonPressed;
	}


	const SDL_Point& positionPressed() const {
		return m_pos_pressed_px;
	}

	const SDL_Point& position() const {
		return m_pos_px;
	}

	std::function<void(Mouse::Button, SDL_Point)> m_callback_onReleased;
	std::function<void(Mouse::Button, SDL_Point)> m_callback_onPressed;
	std::function<void()> m_onMouseWheel;


	SDL_Point m_pos_pressed_px, m_pos_released_px;


private:
	SDL_Point m_pos_px;

	bool m_mousePressed = false;
	bool m_leftButtonPressed = false;
	bool m_rightButtonPressed = false;
};
