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
#include <algorithm>
#include <random>


namespace {
constexpr std::uint32_t DefaultSimulationSeed = 0x57415232u; // "WAR2"
std::mt19937 simulationRng(DefaultSimulationSeed);
std::mt19937 cosmeticRng(std::random_device{}());

int deterministicRange(std::mt19937& engine, int min, int max) {
	if (min > max) std::swap(min, max);
	const std::uint64_t range = static_cast<std::uint64_t>(
		static_cast<std::int64_t>(max) - static_cast<std::int64_t>(min)) + 1u;
	constexpr std::uint64_t engineRange = std::uint64_t{1} << 32;
	const std::uint64_t acceptedRange = engineRange - (engineRange % range);

	std::uint64_t value;
	do {
		value = engine();
	} while (value >= acceptedRange);

	return static_cast<int>(static_cast<std::int64_t>(min) +
		static_cast<std::int64_t>(value % range));
}
}
// http://www.cplusplus.com/reference/random/mersenne_twister_engine/mersenne_twister_engine/


// Generates a random number between min and max (both included)
int PseudoRandom::rnd(int min, int max) {
	// Do not use uniform_int_distribution here: its exact mapping is not
	// specified across standard-library implementations. This rejection
	// sampler produces the same result on every platform for the same seed.
	return deterministicRange(simulationRng, min, max);
}

int PseudoRandom::cosmeticRnd(int min, int max) {
	if (min > max) std::swap(min, max);
	std::uniform_int_distribution<std::mt19937::result_type> r(min, max);
	return r(cosmeticRng);
}

void PseudoRandom::seed(std::uint32_t seedValue) {
	simulationRng.seed(seedValue);
}
