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
#include "PseudoRandom.h"
#include <random>


std::random_device dev;
std::mt19937 rng(dev()); // ToDo: Set seed
// http://www.cplusplus.com/reference/random/mersenne_twister_engine/mersenne_twister_engine/


// Generates a random number between min and max (both included)
int PseudoRandom::rnd(int min, int max) {
	std::uniform_int_distribution<std::mt19937::result_type> r(min, max);
	return r(rng);
}
