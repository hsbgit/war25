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

#include <cmath>
#include <algorithm>

namespace War25::Utils::Math {
    template<typename T>
    constexpr T clamp(T value, T min_val, T max_val) {
        return std::max(min_val, std::min(value, max_val));
    }

    template<typename T>
    constexpr T lerp(T a, T b, float t) {
        return a + t * (b - a);
    }

    constexpr int calculateDistance(int x1, int y1, int x2, int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        return static_cast<int>(std::sqrt(dx * dx + dy * dy));
    }

    constexpr bool isInRange(int value, int min_val, int max_val) {
        return value >= min_val && value <= max_val;
    }

    constexpr int roundToNearestMultiple(int value, int multiple) {
        return ((value + multiple / 2) / multiple) * multiple;
    }
}