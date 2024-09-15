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

/*
 * Used by Player and ResourceManager.
 * Note: Neutral value is special case (See struct PUD_Player)
 *
 * For instance, Dark Portal and GoldMine and Critters do not have different colours, they are always Neutral.
 */
enum class PlayerColor {Red=0, Blue, Green, Violet, Orange, Black, White, Yellow, Neutral=15};


/* 
 * For details about this magic (exact colors and the offset 208)
 * refer to wargus-master/scrips/stratagus.lua and search for "DefinePlayerColorIndex(208, 4)"
 */
#include <SDL.h>
#include <vector>
const std::vector<SDL_Color> redPaletteColors    = {{164, 0, 0, 255}, {124, 0, 0, 255}, {92, 4, 0, 255}, {68, 4, 0, 255}};
const std::vector<SDL_Color> bluePaletteColors   = {{12, 72, 204, 255}, {4, 40, 160, 255}, {0, 20, 116, 255}, {0, 4, 76, 255}};
const std::vector<SDL_Color> greenPaletteColors  = {{44, 180, 148, 255}, {20, 132, 92, 255}, {4, 84, 44, 255}, {0, 40, 12, 255}};
const std::vector<SDL_Color> violetPaletteColors = {{152, 72, 176, 255}, {116, 44, 132, 255}, {80, 24, 88, 255}, {44, 8, 44, 255}};
const std::vector<SDL_Color> orangePaletteColors = {{248, 140, 20, 255}, {200, 96, 16, 255}, {152, 60, 16, 255}, {108, 32, 12, 255}};
const std::vector<SDL_Color> blackPaletteColors  = {{40, 40, 60, 255}, {28, 28, 44, 255}, {20, 20, 32, 255}, {12, 12, 20, 255}};
const std::vector<SDL_Color> whitePaletteColors  = {{224, 224, 224, 255}, {152, 152, 180, 255}, {84, 84, 128, 255}, {36, 40, 76, 255}};
const std::vector<SDL_Color> yellowPaletteColors = {{252, 252, 72, 255}, {228, 204, 40, 255}, {204, 160, 16, 255}, {180, 116, 0, 255}};


#include <map>
const std::map<PlayerColor, std::vector<SDL_Color>> mapPlayerPaletteColors = {
    {PlayerColor::Red, redPaletteColors},
    {PlayerColor::Blue, bluePaletteColors},
    {PlayerColor::Green, greenPaletteColors},
    {PlayerColor::Violet, violetPaletteColors},
    {PlayerColor::Orange, orangePaletteColors},
    {PlayerColor::Black, blackPaletteColors},
    {PlayerColor::White, whitePaletteColors},
    {PlayerColor::Yellow, yellowPaletteColors}};
