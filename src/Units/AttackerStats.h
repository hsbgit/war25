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


class AttackerStats {
public:
	AttackerStats(int basicDamage, int piercingDamage) : m_basicDamage(basicDamage), m_piercingDamage(piercingDamage) {

	}

	inline void setBasicDamage(int basicDamage) {
		m_basicDamage = basicDamage;
	}

	inline void setPiercingDamage(int piercingDamage) {
		m_piercingDamage = piercingDamage;
	}

	inline int getBasicDamage() const {
		return m_basicDamage;
	}

	inline int getPiercingDamage() const {
		return m_piercingDamage;
	}


private:
	int m_basicDamage, m_piercingDamage;
};
