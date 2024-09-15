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
#include "Mouse.h"
#include <iostream>


bool Mouse::consume(SDL_Event& event) {
	switch (event.type) {
		// Only called once when clicked
	case SDL_MOUSEBUTTONDOWN:
	{
		m_pos_px.x = event.button.x;
		m_pos_px.y = event.button.y;


		if (m_callback_onPressed) {

			m_callback_onPressed((event.button.button == SDL_BUTTON_LEFT) ? Button::Left : Button::Right, m_pos_px);
		}



		m_leftButtonPressed = (event.button.button == SDL_BUTTON_LEFT);
		m_rightButtonPressed = (event.button.button == SDL_BUTTON_RIGHT);


		m_pos_pressed_px = m_pos_px;
		m_mousePressed = true;


		//      mouseClick_Tile_x = event.button.x / 32;
		//      mouseClick_Tile_y = event.button.y / 32;
		return true;
	}
	break;

	case SDL_MOUSEBUTTONUP:
	{
		m_pos_px.x = event.button.x;
		m_pos_px.y = event.button.y;

		if (m_callback_onReleased) {


			m_callback_onReleased((event.button.button == SDL_BUTTON_LEFT) ? Button::Left : Button::Right, m_pos_px);
		}

		m_pos_released_px = m_pos_px;

		if (event.button.button == SDL_BUTTON_LEFT)
			m_leftButtonPressed = false;

		if (event.button.button == SDL_BUTTON_RIGHT)
			m_rightButtonPressed = false;

		std::cout << "Button UP!" << std::endl;
		m_mousePressed = false;
		/*
				if(event.button.button == SDL_BUTTON_LEFT) {
					m_leftButtonPressed = false;

					map.m_player.m_setSelectedObjects.clear();
					//mousePressed = false;

					// Wenn left button
					int tile_topleft_x = viewport.m_viewPos_x + mouseClick_x / 32;
					int tile_topleft_y = viewport.m_viewPos_y + mouseClick_y / 32;

					int tile_lowerright_x = viewport.m_viewPos_x + mouseCurrPos_x / 32; // viewport.maxViewableMapHeight()
					int tile_lowerright_y = viewport.m_viewPos_y + mouseCurrPos_y / 32;

					// min max vertauschen wenn von rechts nach links gezogen wird
					for(std::size_t y=tile_topleft_y; y<=tile_lowerright_y; ++y) {
						for(std::size_t x=tile_topleft_x; x<=tile_lowerright_x; ++x) {
							Tile& tile = map.m_arrTileMap[y][x];
							if(tile.hasObject()) {
								if(tile.m_pLandUnit->m_pOwner == &map.m_player) {
									map.m_player.m_setSelectedObjects.insert(tile.m_pLandUnit);
								}
							}
						}
					}

					if(map.m_player.m_setSelectedObjects.size()) {
						menubar.setSelectedObject(*map.m_player.m_setSelectedObjects.begin());
					} else {
						menubar.setSelectedObject(0);
					}
				} else if(event.button.button == SDL_BUTTON_RIGHT) {
					m_rightButtonPressed = false;

					if(map.m_player.m_setSelectedObjects.size()) {
						Object* pObject = *map.m_player.m_setSelectedObjects.begin();
						pObject->m_vecPath.clear();

						// viewport.maxViewableMapHeight()
						Tile& tile = map.m_arrTileMap[viewport.m_viewPos_y +mouseCurrPos_y / 32][viewport.m_viewPos_x + mouseCurrPos_x / 32];


						std::cout<<"Start: "<<std::dec<<pObject->m_pTile->getY()<<","<<pObject->m_pTile->getX()<<std::endl;
						std::cout<<"Ziel : "<<tile.getY()<<","<<tile.getX()<<std::endl;



						AStar astar(pObject->m_pTile, &tile);
						Path path = astar.findPath();
						if(path.size()) {
						// Object->assign(path)
						}

					}
				}
		*/
		return true;
	}
	break;


	case SDL_MOUSEWHEEL:
		if (m_onMouseWheel) {
			m_onMouseWheel();
		}

		return true;
		break;


	case SDL_MOUSEMOTION:
	{
		m_pos_px.x = event.button.x;
		m_pos_px.y = event.button.y;
		return true;
	}
	break;



	default:
		break;
	}

	return false;
}
