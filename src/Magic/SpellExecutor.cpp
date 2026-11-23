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
#include "Player.h"
#include "Map/Map.h"
#include "Magic/Runes.h"
#include "Magic/SpellEffect.h"
#include "Magic/BombardmentEffect.h"
#include "Magic/IMagical.h"
#include "Magic/SpellCosts.h"
#include "Magic/SpellRules.h"
#include "Resources/SoundManager.h"
#include "Units/Critter.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"
#include "Units/LandUnits/Mage.h"
#include "Units/LandUnits/Skeleton.h"
#include "Units/AirUnits/EyeOfKillrogg.h"
#include "Units/CombatRules.h"
#include "Units/Projectile.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <cmath>
#include <vector>


SpellExecutor::SpellExecutor() : EventProcessor() {

}


SpellExecutor::~SpellExecutor() {
    
}


void SpellExecutor::placeRunes(Runes& r, const Point& targetPos_world) {
    Tile* pTile = m_pMap->getTile(targetPos_world);
    if (pTile) {
        if (!pTile->hasObject() && !pTile->isDestroyable() && !pTile->isCoast()) {
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
	castDebugSpell(event.getEventID(), {event.tileX(), event.tileY()});
}

bool SpellExecutor::castSpell(Unit* caster, EventID spell, const Point& targetPos_world) {
	return castSpellInternal(caster, spell, targetPos_world, false);
}

bool SpellExecutor::castDebugSpell(EventID spell, const Point& targetPos_world) {
	if (!m_pMap) return false;
	Unit* caster = nullptr;
	for (Object* object : m_pMap->humanControlledPlayer()->getObjects()) {
		auto* unit = dynamic_cast<Unit*>(object);
		if (!unit || unit->isDead() || !unit->isPlacedOnMap()) continue;
		if (!caster) caster = unit;
		if (dynamic_cast<IMagical*>(unit)) {
			caster = unit;
			break;
		}
	}
	if (!caster) {
		if (Tile* target = m_pMap->getTile(targetPos_world))
			caster = dynamic_cast<Unit*>(target->getAirUnit()
				? target->getAirUnit() : target->getSeaOrLandUnit());
	}
	if (!caster) return false;
	return castSpellInternal(caster, spell, targetPos_world, true);
}

bool SpellExecutor::castSpellInternal(Unit* caster, EventID spell,
	const Point& targetPos_world, bool debugCast) {
    assert(m_pMap);
    std::cerr << "SpellExecutor called with event id " << std::dec << (int)spell << std::endl;

    Point m_targetPos_world = targetPos_world;
    Tile* pTargetTile = m_pMap->getTile(m_targetPos_world);
	if (!pTargetTile) return false;
	auto unitOnTarget = [pTargetTile]() -> Unit* {
		Object* object = pTargetTile->getAirUnit()
			? pTargetTile->getAirUnit() : pTargetTile->getSeaOrLandUnit();
		return dynamic_cast<Unit*>(object);
	};

	auto* mana = caster ? dynamic_cast<IMagical*>(caster) : nullptr;
	const int fixedCost = SpellCosts::manaCost(static_cast<int>(spell));
	if (caster && !debugCast) {
		const bool variableCost = spell == static_cast<EventID>(PaladinActions::Heal);
		if (!caster->isPlacedOnMap() || !mana ||
			(!variableCost && (fixedCost < 0 || mana->getMana() < fixedCost))) return false;
		const int range = SpellCosts::range(static_cast<int>(spell));
		if (range >= 0) {
			const Point topLeft = caster->getTileTopLeft()->getPos();
			const Point lowerRight = caster->getTileLowerRight()->getPos();
			if (!CombatRules::rectanglesWithinMapRange(
				topLeft.x, topLeft.y, lowerRight.x, lowerRight.y,
				targetPos_world.x, targetPos_world.y,
				targetPos_world.x, targetPos_world.y, range)) return false;
		}
	}
	bool applied = false;

    // Very good literature: http://classic.battle.net/war2/advanced/statistics.shtml 


    switch ((int)spell) {
	case (int)PaladinActions::HolyVision:
		if (!caster) return false;
		caster->getOwner()->getMapInfo()->revealArea(targetPos_world, 4, 2000);
		g_pSoundManager->playWAV("spells/holy_vision.wav");
		m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
			ResourceManager_wartools::SpellVisual::Normal).playOnce());
		applied = true;
		break;

	case (int)PaladinActions::Heal:
	{
		if (!caster || (!debugCast && !mana)) return false;
		Unit* target = unitOnTarget();
		if (!target || target->isDead() || !target->isOrganic() ||
			target->isEnemy(caster)) return false;
		const int missingHealth = target->getMaxHealthPoints() - target->getHealthPoints();
		if (missingHealth <= 0) return false;
		if (debugCast) {
			target->increaseHealth(missingHealth);
		} else {
			const auto result = SpellCosts::healing(mana->getMana(), missingHealth);
			if (result.health <= 0 || !mana->reduceMana(result.mana)) return false;
			target->increaseHealth(result.health);
		}
		m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
			ResourceManager_wartools::SpellVisual::Healing).playOnce());
		caster->removeBuff(BuffType::Invisibility);
		g_pSoundManager->playWAV("spells/healing.wav");
		return true;
	}

	case (int)OgreActions::EyeOfKillrogg:
	{
		if (!caster) return false;
		Tile* spawnTile = m_pMap->getNextFreeTile(caster->getTile(), 2);
		if (!spawnTile) return false;
		auto* eye = new EyeOfKillrogg(caster->getOwner(), m_pMap);
		m_pMap->placeUnit(spawnTile, eye);
		g_pSoundManager->playWAV("spells/eye_of_kilrogg.wav");
		m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
			ResourceManager_wartools::SpellVisual::Normal).playOnce());
		applied = true;
	}
	break;

    case (int)BuffType::Bloodlust:
    {
        Unit* pUnit = unitOnTarget();

        if (pUnit && pUnit->isOrganic()) {
			pUnit->addBuff(Buff(BuffType::Bloodlust));
			m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::Normal).playOnce());
            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "bloodlust.wav");
			applied = true;
        }
    }
    break;

    case (int)BuffType::Slow:
    {
        // Slow can be cast on any unit. 
        // The effect is movement speed and attack speed are cut in half.
        // Slow slows down any unit attack by half.
        // http://classic.battle.net/war2/units/mage.shtml
        Unit* pUnit = unitOnTarget();

        if (pUnit) {
			pUnit->addBuff(Buff(BuffType::Slow));
			m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::Normal).playOnce());
            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "slow.wav");
			applied = true;
        }
    }
    break;

    case (int)BuffType::Haste:
    {
        Unit* pUnit = unitOnTarget();

        if (pUnit) {
			pUnit->addBuff(Buff(BuffType::Haste));
			m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::Normal).playOnce());
            g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "haste.wav");
			applied = true;
        }
    }
    break;


    case (int)BuffType::Invisibility:
    {
        // For each cast of Invisibility one unit is made invisible.
        // This can be cast on your units, teammates units, and even enemy units although that is not very wise.
        // The unit cannot be seen by anyone else except yourself and your teammates.
        // The unit remains invisible until the duration runs out or the unit attacks or casts any spells which will instantly reveal it.
        // Invisible Peasant can build a building and remain Invisible after the buildings competition.
        // http://classic.battle.net/war2/units/mage.shtml
        Unit* pUnit = unitOnTarget();

        if (pUnit) {

            if (typeid(*pUnit) == typeid(Dwarfes)) {
				dynamic_cast<Dwarfes*>(pUnit)->detonateWithoutAreaDamage();
            }
            else {
                pUnit->addBuff(Buff(BuffType::Invisibility));
            }

			g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "invisibility.wav");
			m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::Normal).playOnce());
			applied = true;
        }
    }
    break;



    case (int)PaladinActions::Excorcism:
    {
        Unit* pUnit = unitOnTarget();

        if (pUnit && pUnit->isOrganic()) {

            if (typeid(*pUnit) == typeid(Mage) && pUnit->getFraction() == Fraction::Orc) {
                pUnit->die();
                g_pSoundManager->playWAV(std::string("spells") + PATH_SEPARATOR + "exorcism.wav");
				applied = true;
            }
        }
    }
    break;


    case (int)BuffType::UnholyArmor:
    {
        Unit* pUnit = unitOnTarget();

        if (pUnit) {
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
			m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::Normal).playOnce());
			applied = true;
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
			applied = true;
        }
    }
    break;

	case (int)MageActions::Fireball:
	{
		if (!caster || !caster->isPlacedOnMap()) break;
		const Point origin = caster->getTileLocation();
		const int dx = (m_targetPos_world.x > origin.x) - (m_targetPos_world.x < origin.x);
		const int dy = (m_targetPos_world.y > origin.y) - (m_targetPos_world.y < origin.y);
		if (dx == 0 && dy == 0) break;
		const int travelRange = SpellCosts::range(static_cast<int>(MageActions::Fireball));
		Point destination{origin.x + dx * travelRange, origin.y + dy * travelRange};
		destination.x = std::clamp(destination.x, 0, m_pMap->sideLength() - 1);
		destination.y = std::clamp(destination.y, 0, m_pMap->sideLength() - 1);
		m_pMap->addMapObject(new Projectile(
			ResourceManager_wartools::MissileType::Fireball, caster, m_pMap,
			destination, AttackerStats(0, SpellRules::FireballDamage), true));
		applied = true;
	}
	break;

	case (int)MageActions::FlameShield:
	{
		Unit* target = unitOnTarget();
		if (target && !target->isAirObject()) {
			const Animation flames = g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::FlameShield);
			for (int flame = 0; flame < SpellRules::FlameShieldFlames; ++flame) {
				m_pMap->addMapObject(new SpellEffect(caster, m_pMap, target->getTileLocation(),
					flames.singleFrame(static_cast<std::size_t>(flame) % flames.frameCount()),
					SpellRules::FlameShieldDurationMs,
					flame == 0 ? SpellRules::FlameShieldDamagePerPulse : 0,
					1, SpellEffect::Motion::OrbitTarget, target, false, 1000, flame));
			}
			g_pSoundManager->playWAV("spells/flame_shield.wav");
			applied = true;
		}
	}
	break;

	case (int)MageActions::Blizzard:
		for (int cycle = 0; cycle < SpellRules::blizzardCycles(debugCast); ++cycle)
		for (int shard = 0; shard < SpellRules::BombardmentShards; ++shard) {
			const auto impact = SpellRules::bombardmentTile(
				m_targetPos_world, shard, m_pMap->sideLength());
			m_pMap->addMapObject(new BombardmentEffect(caster, m_pMap,
				{impact.x, impact.y},
				g_pRessourceManager->getSpellAnimation(ResourceManager_wartools::SpellVisual::Blizzard),
				SpellRules::bombardmentDamage(shard, m_targetPos_world),
				cycle * SpellRules::BlizzardCycleDelayMs +
					shard * SpellRules::BombardmentShardDelayMs,
				SpellRules::BlizzardFallDurationMs,
				SpellRules::BlizzardFallHeightTiles));
		}
		g_pSoundManager->playWAV("spells/blizzard.wav");
		applied = true;
		break;

	case (int)DeathKnightActions::DeathCoil:
	{
		Unit* victim = unitOnTarget();
		if (!caster || !victim || victim->isDead() || !victim->isOrganic() ||
			!victim->isEnemy(caster)) break;
		m_pMap->addMapObject(new Projectile(
			ResourceManager_wartools::MissileType::TouchOfDeath, caster, m_pMap,
			victim, AttackerStats(0, 50)));
		g_pSoundManager->playWAV("spells/death_coil.wav");
		applied = true;
	}
	break;

	case (int)DeathKnightActions::Whirlwind:
		m_pMap->addMapObject(new SpellEffect(caster, m_pMap, m_targetPos_world,
			g_pRessourceManager->getSpellAnimation(ResourceManager_wartools::SpellVisual::Whirlwind),
			SpellRules::WhirlwindDurationMs, SpellRules::WhirlwindDamagePerPulse, 0,
			SpellEffect::Motion::Whirlwind, nullptr, false,
			SpellRules::WhirlwindDamageIntervalMs, 0,
			SpellRules::WhirlwindSoundIntervalMs, "spells/whirlwind.wav"));
		g_pSoundManager->playWAV("spells/whirlwind.wav");
		applied = true;
		break;

	case (int)DeathKnightActions::RaiseTheDead:
	{
		Tile* secondTile = nullptr;
		for (Tile* candidate : m_pMap->getNeighbourTiles(pTargetTile, 2)) {
			if (m_pMap->isSpaceAvailableForUnit(candidate->getY(), candidate->getX(),
				1, Object::Type::Land)) {
				secondTile = candidate;
				break;
			}
		}
		if (caster && !pTargetTile->getSeaOrLandUnit() && secondTile &&
			m_pMap->consumeRaiseableCorpse(pTargetTile)) {
			m_pMap->placeUnit(pTargetTile, new Skeleton(caster->getOwner(), m_pMap));
			m_pMap->placeUnit(secondTile, new Skeleton(caster->getOwner(), m_pMap));
			g_pSoundManager->playWAV("spells/touch_of_darkness.wav");
			applied = true;
		}
	}
	break;

	case (int)DeathKnightActions::DeathAndDecay:
		for (int shard = 0; shard < SpellRules::BombardmentShards; ++shard) {
			const auto impact = SpellRules::decayTile(
				m_targetPos_world, shard, m_pMap->sideLength());
			m_pMap->addMapObject(new BombardmentEffect(caster, m_pMap,
				{impact.x, impact.y},
				g_pRessourceManager->getSpellAnimation(ResourceManager_wartools::SpellVisual::DeathAndDecay),
				SpellRules::bombardmentDamage(shard, m_targetPos_world),
				SpellRules::decayDelayMs(shard),
				900, 0));
		}
		g_pSoundManager->playWAV("spells/death_and_decay.wav");
		applied = true;
		break;

    case (int)MageActions::Polymorph:
    {
        Unit* pUnit = unitOnTarget();
        if (pUnit && pUnit->isOrganic() && pTargetTile->isLand()) {
            // This turns any living creature into the critter for that map type such as Pig for Wasteland, Seal for Snow, Sheep for Forest, Boar for Swamp. 
            // There is no warning to enemy players that their units have been Polymorphs.
            // http://classic.battle.net/war2/units/mage.shtml
            // Since we must place the critter we have to disconnect pUnit now from its tile
            m_pMap->releaseObjectFromTile(pUnit);

            // Welcome new animal
            Critter* critter = new Critter(m_pMap->getNeutralPlayer(), m_pMap);
            m_pMap->placeUnit(pTargetTile, critter);
            g_pSoundManager->playWAV("spells/polymorph.wav");
			m_pMap->addAnimation(pTargetTile, g_pRessourceManager->getSpellAnimation(
				ResourceManager_wartools::SpellVisual::Normal).playOnce());
			applied = true;
        }
    }
    break;

    default:
		return false;
    }
	if (!applied) return false;
	if (caster && !debugCast) {
		if (!mana->reduceMana(fixedCost)) return false;
		caster->removeBuff(BuffType::Invisibility);
	}
	return true;
}


void SpellExecutor::setMap(Map* pMap) {
	vecRunes.clear();
	m_pMap = pMap;
}

SpellExecutor gSpellExecutor;
