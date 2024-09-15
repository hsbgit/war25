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


enum class GeneralActions { Move, Stop, Attack, Patrol, StandGround };


enum class PeasantActions {
	Repair = 100, Harvest, Build_Stage1, Build_Stage2, HarvestGold, HarvestWood, HarvestOil,
	BuildBegin, BuildTownhall, BuildFarm, BuildBarracks, BuildLumberMill, BuildBlacksmith, BuildChurch, BuildStables, BuildGnomishInventor, BuildGryphonAviary, BuildScoutTower, BuildShipyard, BuildFoundry, BuildOilRefinery, BuildEnd
};


// Buffs such as BloodLust are not part of the OgreActions, they belong to the class 'Buff'
enum class MageActions { Fireball = 200, FlameShield, Polymorph, Blizzard };
enum class DeathKnightActions { DeathCoil = 300, Whirlwind, RaiseTheDead, DeathAndDecay };
enum class PaladinActions { HolyVision = 400, Heal, Excorcism };
enum class OgreActions { EyeOfKillrogg = 500, Runes };
enum class DwarfesActions { Explode = 600 };

enum class BuffType { Uninitialized = 700, Bloodlust, Haste, Slow, Invisibility, UnholyArmor };