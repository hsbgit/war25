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
#include "Object.h"
#include "Player.h"
#include "Units/IArmed.h"
#include "TechnologyManager.h"
#include "Resources/SoundManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Map/Map.h"
#include <cassert>
#include <iostream>


Object::Object(Player* pOwner, Map* pMap, int healthPointsMax, int armor, int sight) : m_pOwner(pOwner), m_pMap(pMap), m_healthPoints(healthPointsMax), m_healthPointsMax(healthPointsMax), m_armor(armor), m_sight(sight), m_icon(g_pRessourceManager->getUISprite("orc//cursors//red_crosshairs.png")) {
	if (!m_pOwner) {
		throw std::logic_error("Each object must have an owner!");
	}

	// Add to player unit list
	m_pOwner->addObject(this);
	m_pOwner->getTechnologyCoordinator()->installUpgrades(this);
}


Object::~Object() {
	std::cerr << "Object::~Object()" << std::endl;
	getMap()->releaseObjectFromTile(this);
}


bool Object::isEnemy(Object* pPlayer) const {
	return (m_pOwner != pPlayer->m_pOwner);
}

#include "Global/PseudoRandom.h"
void Object::onClicked() {
	playSoundSelected();
}


#include "Global/PseudoRandom.h"


// https://forum.dune2k.com/topic/26794-warcraft-2-incorrect-information/
// http://classic.battle.net/war2/basic/combat.shtml
// http://web.archive.org/web/19970705002242/http://www.blizzard.com/tips/tip2.htm 
// noRandomComponent: If done by a spell, e.g., ogre mage runes or whirlwind or demolition dwarfes, it should be exactly the specified damage. 
void Object::takeDamage(const AttackerStats& stats, bool noRandomComponent) {
	bool alreadyDead = isDead(); // Further arrows or whatever may be fyling/incoming...
	if (alreadyDead)
		return; // We can stop here, it is already dead...

	// (Basic Damage - Target's Armor) + Piercing Damage = Maximum damage inflicted
	int basicDamage_hp = std::max(0, stats.getBasicDamage() - getArmor()); // May get negative, in that case we set the basic damage to zero
	int piercingDamage_hp = stats.getPiercingDamage();
	int potentialDamage_hp = basicDamage_hp + piercingDamage_hp;

	//The attacker does a random amount of damage from 50% up to 100% of this total each attack.
	int actualDamage = noRandomComponent ? potentialDamage_hp : PseudoRandom::rnd(potentialDamage_hp / 2, potentialDamage_hp);
	m_healthPoints -= actualDamage;

	// Splash damage: https://forum.war2.ru/index.php?topic=4938.0

	if (!alreadyDead && isDead()) {
		setDeathProperties();
	}
}


void Object::setDeathProperties() {
	assert(isDead());

	if (m_strDyingSound.size())
		g_pSoundManager->playWAV(this, m_strDyingSound);

	notifyAttackersThatImDead();
	m_sight = 1;

	// Case 1: Object is vanishing/rotting -> make tile free but keep object associated with tile such that the rotten object can be re-surrected as a skeleton
	// Case 2: Objects that do not vanish and that can not be re-surrected as skeletons: Ballista, Mage, Dragon, ...
   // m_pMap->removeObject(this);

	// todo: no dying sound when dwarf
	// todo: if biological place corpse

	// All subclasses must handle their death
	doHandleDeath();
}


void Object::notifyAttackersThatImDead() {
	for (auto& pAttacker : m_vecAttackers) {
		pAttacker->releaseActiveTarget();
	}
}

