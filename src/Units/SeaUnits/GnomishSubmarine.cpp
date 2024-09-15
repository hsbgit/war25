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
#include "GnomishSubmarine.h"
#include "Resources/resourcemanager_wartools.h"
#include "Player.h"


GnomishSubmarine::GnomishSubmarine(Player* pOwner, Fraction fraction, Map* pMap) : Unit(pOwner, fraction, pMap, 60, 0, 5, 7), IArmed(50, 0, 4) {
    // Generic behavior
    if (m_fraction == Fraction::Alliance) {
        setSoundBaseDirectory("human/ships/");

        // Submarine has its own annoyed sounds
        m_vecSoundsAnnoyed.clear();
        m_vecSoundsAnnoyed.push_back("human/ships/gnomish_submarine/annoyed/1.wav");
        m_vecSoundsAnnoyed.push_back("human/ships/gnomish_submarine/annoyed/2.wav");
        m_vecSoundsAnnoyed.push_back("human/ships/gnomish_submarine/annoyed/3.wav");
        m_vecSoundsAnnoyed.push_back("human/ships/gnomish_submarine/annoyed/4.wav");
    }
    else {
        setSoundBaseDirectory("orc/ships/");
    }

    setDyingSound("ships/sinking.wav");

    // Specific behavior
    m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
    m_icon = g_pRessourceManager->getIcon(PUD_ICON_GNOMISH_SUBMARINE, this->getFraction(), getOwner()->getColor());

    setName((fraction == Fraction::Alliance) ? "Gnomish Submarine" : "Giant Turtle");
}
