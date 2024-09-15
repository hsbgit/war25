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
#include "SpellExecutor.h"
#include "Map/Map.h"
#include "Magic/Runes.h"
#include "Resources/SoundManager.h"
#include "Units/Critter.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"
#include "Units/LandUnits/Mage.h"
#include <iostream>


SpellExecutor::SpellExecutor() : EventProcessor() {

}


SpellExecutor::~SpellExecutor() {
    
}


void SpellExecutor::placeRunes(Runes& r, const Point& targetPos_world) {
    Tile* pTile = m_pMap->getTile(targetPos_world);
    if (pTile) {
        if (pTile->hasObject() && pTile->getSeaOrLandUnit()) {
            pTile->getSeaOrLandUnit()->takeDamage(AttackerStats(0, 50), true);
            g_pSoundManager->playWAV(pTile, std::string("misc") + PATH_SEPARATOR + "explosion.wav");
            m_pMap->addAnimation(pTile, g_pRessourceManager->getExplosionAnimation());
        }
        else if (!pTile->isDestroyable() && !pTile->isCoast()) {
            // todo: just reduce mana if rune can be placed
            r.vecRuneTiles.push_back(pTile);
        }
    }
}


void SpellExecutor::process() {
    for (Runes& r : vecRunes) {
        r.process();
    }


    vecRunes.erase(std::remove_if(vecRunes.begin(), vecRunes.end(), [](const Runes& rune) {
        return rune.expired(); }),
        vecRunes.end());
}



void SpellExecutor::handleEvent(const Event& event) {
    assert(m_pMap);
    std::cerr << "SpellExecutor called with event id " << std::dec << (int)event.getEventID() << std::endl;

    Point m_targetPos_world;
    m_targetPos_world.x = event.tileX();
    m_targetPos_world.y = event.tileY();
    Tile* pTargetTile = m_pMap->getTile(m_targetPos_world);

    // Very good literature: http://classic.battle.net/war2/advanced/statistics.shtml 


    // ToDo: Death Coil
    // When cast, you will lose mana even if no unit gets hit by the Death Coil.
    // When multiple enemies are hit by Death Coil, the damage is distributed between them all.So the more enemies, the less damage it does.
    // Cast a Death coil on a Sheep, Pig, Seal or Boar to heal your Death Knight 5 hit points. 
    // Source: http://classic.battle.net/war2/units/deathknight.shtml 


    // ToDo: Whirlwind
    // "The Whirlwind tip must be exactly on a building to do any damage."
    // Source: http://classic.battle.net/war2/units/deathknight.shtml 


    // ToDo: Blizzard
    // A Blizzard hits a 5x5 area with minimal damage of 7x7.When cast on a building it centers over the top left of the building.
    // http://classic.battle.net/war2/advanced/bdnd.shtml


    // ToDo: Flame Shield
    // You cannot cast Flame Shield on air units such as Flying Machines and Gryphons.
    // Because Flame Shield can damage air units, cast it on ground units then have them run underneath the enemy flying units. 
    // Cast a Flame Shield on Peasants mining Gold.The Flame Shield will hit other Peasants walking to and from the Gold Mine.
    // http://classic.battle.net/war2/units/mage.shtml


    // ToDo: Fireball
    // Launches a Fireball in one direction for approximately 8 spaces that has the ability to hit land, sea and air targets in the path. 
    // The damage across that line varies but in some tests the average was about 34 damage.
    // Units in the path of the Fireball received greater or less than 34. 
    // The tip of the Fireball does the least amount of damage as units hit with it was usually less than 10 hit points.
    // http://classic.battle.net/war2/units/mage.shtml


    // ToDo: Raise the Dead
    // Source: http://classic.battle.net/war2/units/deathknight.shtml 
    // http://classic.battle.net/war2/advanced/bdnd.shtml



    switch ((int)event.getEventID()) {
    case (int)BuffType::Bloodlust:
    {
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?

        if (pUnit && pUnit->isOrganic()) {
            pUnit->addBuff(Buff(BuffType::Bloodlust));
            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "bloodlust.wav");
        }
    }
    break;

    case (int)BuffType::Slow:
    {
        // Slow can be cast on any unit. 
        // The effect is movement speed and attack speed are cut in half.
        // Slow slows down any unit attack by half.
        // http://classic.battle.net/war2/units/mage.shtml
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?

        if (pUnit && pUnit->isOrganic()) {
            pUnit->addBuff(Buff(BuffType::Slow));
            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "slow.wav");
        }
    }
    break;

    case (int)BuffType::Haste:
    {
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?

        /*
         * ToDo:
         * "Haste increases the speed of most units, however, there are some units that Haste does not affect."
         *
         * "Haste does NOT noticeably increase attack speed or the number of attacks except in the case of Dragons.
         *  Haste makes it appear through the animation, that units are attacking faster but actually they are not."
         *
         * Source: http://classic.battle.net/war2/units/deathknight.shtml
         */
        if (pUnit && pUnit->isOrganic()) {
            // ToDo: Handle haste effect
           /*
            Land
            Peon/Peasant = 13 (Same as Ogre)
            Grunt/Footman = 13 (Same as Ogre)
            Axe thrower/Archer = 13 (Same as Ogre)
            Ogres/Knights = no effect
            Catapult/Ballista = 9 (Slower than a Grunt, Faster than a Mage/DK)
            Sappers/Demos = Speed 17! (Same as Goblin Zeppelin)
            Death Knights = Speed 17! (Same is Goblin Zeppelin)
            Mage = Speed 13. So Hasted Death Knights are faster than Hasted Mages.
            Air
            Zeppelins/Flying Machines = no effect
            Dragons/Gryphons = Faster than speed 17, about 21+ speed.

            Sea
            Tankers = no effect
            Transports = no effect
            Destroyers = no effect
            Juggernaughts/Battleships = Speed 10 (Same as Oil Tanker, Transport, Destroyers)
            Subs = Speed 10 (Same as tanker, trans, destroyers)
            */
            pUnit->addBuff(Buff(BuffType::Haste));
            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "haste.wav");
        }
    }
    break;


    case (int)BuffType::Invisibility:
    {
        // ToDo: Object attribute "isVisible"? Submarines are never visible but a flying machine can see them
        // For each cast of Invisibility one unit is made invisible.
        // This can be cast on your units, teammates units, and even enemy units although that is not very wise.
        // The unit cannot be seen by anyone else except yourself and your teammates.
        // The unit remains invisible until the duration runs out or the unit attacks or casts any spells which will instantly reveal it.
        // Invisible Peasant can build a building and remain Invisible after the buildings competition.
        // http://classic.battle.net/war2/units/mage.shtml
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?

        if (pUnit && pUnit->isOrganic()) {

            if (typeid(*pUnit) == typeid(Dwarfes)) {
                pUnit->die();
            }
            else {
                pUnit->addBuff(Buff(BuffType::Invisibility));
            }

            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "invisibility.wav");
        }
    }
    break;



    case (int)PaladinActions::Excorcism:
    {
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?

        if (pUnit && pUnit->isOrganic()) {

            if (typeid(*pUnit) == typeid(Mage) && pUnit->getFraction() == Fraction::Orc) {
                pUnit->die();
                g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "exorcism.wav");
            }
        }
    }
    break;


    case (int)BuffType::UnholyArmor:
    {
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?

        if (pUnit && pUnit->isOrganic()) {
            if (typeid(*pUnit) == typeid(Dwarfes)) {
                 // You can also cast Unholy Armor on Demolition Squads and Goblin Sappers to blow them up.
                 // Unholy Armor's main weakness in the short time that it lasts. Often what the enemy will do is see Unholy Armor, run until the spell wears off and you're left with a unit with 1/2 hit points.
                 // Source: http://classic.battle.net/war2/units/deathknight.shtml
                pUnit->die();
            }
            else {
                int hp_dmg = pUnit->getHealthPoints();
                hp_dmg /= 2; // Price of unholy armor. Unholy armor works best if current hp is 1 as 1/2 = 0 (int) -> no damage
                pUnit->takeDamage(AttackerStats(0, hp_dmg), true);
                pUnit->addBuff(Buff(BuffType::UnholyArmor));
            }

            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "unholy_armor.wav");
        }
    }
    break;

    case (int)OgreActions::Runes:
    {
        Runes r(m_pMap);
        placeRunes(r, Point{ m_targetPos_world.x - 1, m_targetPos_world.y });
        placeRunes(r, Point{ m_targetPos_world.x + 1, m_targetPos_world.y });
        placeRunes(r, Point{ m_targetPos_world.x, m_targetPos_world.y });
        placeRunes(r, Point{ m_targetPos_world.x, m_targetPos_world.y - 1 });
        placeRunes(r, Point{ m_targetPos_world.x, m_targetPos_world.y + 1 });

        if (r.vecRuneTiles.size()) {
            vecRunes.push_back(r);
        }
    }
    break;

    case (int)MageActions::Polymorph:
    {
        Unit* pUnit = dynamic_cast<Unit*>(pTargetTile->getSeaOrLandUnit()); // ToDo: Return Unit because it is always a unit?
        if (pUnit && pUnit->isLandObject()) {
            // ToDo: Dragons yes, but what if dragons are on water?
            // This turns any living creature into the critter for that map type such as Pig for Wasteland, Seal for Snow, Sheep for Forest, Boar for Swamp. 
            // There is no warning to enemy players that their units have been Polymorphs.
            // http://classic.battle.net/war2/units/mage.shtml
            if (pUnit->isOrganic()) {
                // Since we must place the critter we have to disconnect pUnit now from its tile
                m_pMap->releaseObjectFromTile(pUnit);

                // Welcome new animal
                Critter* critter = new Critter(m_pMap->getNeutralPlayer(), m_pMap);
                m_pMap->placeUnit(pTargetTile, critter);
                g_pSoundManager->playWAV("spells/polymorph.wav");
            }
        }
    }
    break;

    default:
        assert(0);
    }
}


void SpellExecutor::setMap(Map* pMap) {
    m_pMap = pMap;
}

SpellExecutor gSpellExecutor;
