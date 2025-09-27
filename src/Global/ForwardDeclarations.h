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

// Core forward declarations to reduce header dependencies

// Game Objects
class Object;
class Unit;
class Building;
class Player;
class Map;
class Tile;

// Resources & Graphics
class Sprite;
class Animation;
class Texture;
class ResourceManager;
class ResourceManager_wartools;
class SoundManager;

// Game Logic
class Action;
enum class Fraction;
enum class PlayerColor;

// GUI
namespace gui {
    class ImGuiWindow;
    class ObjectInfoWindow;
    class DisplaySettingsWindow;
    class MiniMap;
}

// Events
class EventProcessor;
class EventBroker;

// Utils
struct Point;
struct Rect;
struct AttackerStats;

// STL Forward Declarations
#include <vector>
#include <string>
#include <memory>
#include <set>

// Common smart pointer aliases
using ObjectPtr = std::unique_ptr<Object>;
using UnitPtr = std::unique_ptr<Unit>;
using BuildingPtr = std::unique_ptr<Building>;
using TilePtr = std::unique_ptr<Tile>;