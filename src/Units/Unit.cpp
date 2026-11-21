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
#include "Unit.h"
#include "Player.h"
#include "Map/Path.h"
#include "Map/Map.h"
#include "Map/AStar.h"
#include "Global/PseudoRandom.h"
#include "Global/GlobalAttributes.h"
#include "Resources/SoundManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "EventHandling/StateCondition.h"
#include "EventHandling/StateIdle.h"
#include "EventHandling/StateMoving.h"
#include "EventHandling/StateDead.h"
#include "Units/Projectile.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"
#include "Units/LandUnits/Footman.h"
#include "Units/LandUnits/Mage.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Transport.h"
#include <iostream>
#include <filesystem>


#include "EventHandling/StatePatrol.h"



// Fraction may differ from the fraction of pOwner. E.g., an alliance player owning a DeathKnight
Unit::Unit(Player* pOwner, Fraction fraction, Map* pMap, int healthPointsMax, int armor, int sight, int speed) : Object(pOwner, pMap, healthPointsMax, armor, sight), EventProcessor(), m_speed(speed) {
	m_fraction = fraction; // Todo: refactoring

	 // currentAnimation = pResManager->getUnitIdleAnimation(this, Direction::N); Does not work at this point since the object is currently created and unitidle works with virtual (typeid)
	//Object::setWaitTime(300); // Todo: individual
	getOwner()->increaseUsedFood();

	// Setup StateMachine
	m_pIdle = new StateIdle(this);
	m_pMoving = new StateMoving(this);
	m_pPatrol = new CompositeStatePatrol(this);
	m_pDead = new StateDead(this);

	m_pIdle->addTransition(EventID(GeneralActions::Move), m_pMoving);
	m_pIdle->addTransition(EventID(GeneralActions::Patrol), m_pPatrol);
	m_pIdle->addTransition(EventID(GeneralActions::Attack), m_pMoving);

	if (dynamic_cast<IArmed*>(this)) {
		//   std::cerr << "Is armed: " << typeid(*this).name() << std::endl;

		auto f_targetInRange = [this]() {
			IArmed* pArmed = dynamic_cast<IArmed*>(this);
			assert(pArmed); // Must always be valid as this lambda is used to transit into the StateAttack
			return pArmed->hasTargetInRange(this);
		};

		new ConditionBoolean(f_targetInRange);
	}
	else {
		//    std::cerr << "Is NOT armed: " << typeid(*this).name() << std::endl;
	}




	m_pMoving->addTransition(EventID(GeneralActions::Attack), m_pMoving);
	m_pMoving->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pMoving->addTransition(EventID(GeneralActions::Move), m_pMoving); // If a unit is already moving and another move event occurs


	m_pPatrol->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pPatrol->addTransition(EventID(GeneralActions::Move), m_pMoving);
	m_pPatrol->addTransition(EventID(GeneralActions::Attack), m_pMoving);


	auto f_isdead = [](int a, int b) {
		return a < b;
	};
	m_pIdle->addTransition(new ConditionThreshold(this->m_healthPoints, 0, f_isdead), m_pDead);
	m_pMoving->addTransition(new ConditionThreshold(this->m_healthPoints, 0, f_isdead), m_pDead);
	m_pPatrol->addTransition(new ConditionThreshold(this->m_healthPoints, 0, f_isdead), m_pDead);


	m_pMoving->setNextState(m_pIdle);

	m_stateMachine = StateMachine(m_pIdle);
	//   setIdleAnimation();
}

int Unit::getSpeed() const {
	if (hasBuff(BuffType::Slow))
		return std::max(1, m_speed / 2);
	if (!hasBuff(BuffType::Haste))
		return m_speed;

	// Warcraft II's Haste uses unit-specific speeds instead of a multiplier.
	if (dynamic_cast<const Dwarfes*>(this)) return 17;
	if (dynamic_cast<const Mage*>(this))
		return getFraction() == Fraction::Orc ? 17 : 13;
	if (dynamic_cast<const Ballista*>(this)) return 9;
	if (dynamic_cast<const GryphonRider*>(this)) return 21;
	if (dynamic_cast<const Battleship*>(this) || dynamic_cast<const GnomishSubmarine*>(this))
		return 10;
	if (dynamic_cast<const Peasant*>(this) || dynamic_cast<const Footman*>(this) ||
		dynamic_cast<const Archer*>(this)) return 13;
	return m_speed;
}


void Unit::handleEvent(const Event& event) {
	std::cerr << "Unit: Received Event ID: " << std::dec << (int)event.getEventID() << std::endl;
	// Workers and tankers are temporarily removed from the map while they are
	// inside a mine or delivery building. Starting a movement state in that
	// interval would dereference an empty occupied-tile list.
	if (!isPlacedOnMap())
		return;
	if (static_cast<GeneralActions>(event.getEventID()) == GeneralActions::Move &&
		isLandObject()) {
		Object* clicked = getMap()->getObjectOnTile({event.tileX(), event.tileY()});
		auto* transport = dynamic_cast<Transport*>(clicked);
		if (transport && transport->getOwner() == getOwner() && transport->requestBoard(this)) {
			if (Tile* approach = getMap()->getFreeTileAroundBuilding(transport, this))
				m_stateMachine.handleEvent(Event(getID(), GeneralActions::Move,
					approach->getX(), approach->getY()));
			return;
		}
	}
	if (IArmed* armed = dynamic_cast<IArmed*>(this)) {
		const auto action = static_cast<GeneralActions>(event.getEventID());
		if (action == GeneralActions::StandGround) {
			m_standGround = true;
			armed->clearActiveTarget();
			m_stateMachine.handleEvent(Event(getID(), GeneralActions::Stop));
			return;
		}
		if (action == GeneralActions::Attack) {
			m_standGround = false;
			removeBuff(BuffType::Invisibility);
			Object* target = getMap()->getObjectOnTile({event.tileX(), event.tileY()});
			if (target && target != this && target->isEnemy(this) &&
				armed->canAttackTarget(target) &&
				!target->getOwner()->isNeutral()) {
				armed->assignTarget(target);
				if (armed->activeTargetInRange(this)) {
					// Stop an existing walk without clearing the target we just set.
					m_stateMachine.handleEvent(Event(getID(), GeneralActions::Stop));
					return;
				}
				if (Tile* approach = getMap()->getFreeTileAroundBuilding(target, this)) {
					m_stateMachine.handleEvent(Event(getID(), GeneralActions::Attack,
						approach->getX(), approach->getY()));
					return;
				}
				return;
			} else {
				return;
			}
		} else if (action == GeneralActions::Move || action == GeneralActions::Stop) {
			if (action == GeneralActions::Move) m_standGround = false;
			armed->clearActiveTarget();
		}
	}
	m_stateMachine.handleEvent(event);
}


bool Unit::isMoving() const {
	return m_stateMachine.isCurrentStateMovingState();
}


std::vector<Action> Unit::getActions() const {
	std::vector<Action> vecActions;

	PlayerColor color = getOwner()->getColor();
	EventProcessorID id = EventProcessor::getID();

	vecActions.push_back(Action(id, (EventID)GeneralActions::Move, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_MOVE, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)GeneralActions::Stop, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_SHIELD_0, getFraction(), color)));
	vecActions.push_back(Action(id, (EventID)GeneralActions::Attack, g_pRessourceManager->getIcon(PUD_ICON_SWORD_0, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)GeneralActions::Patrol, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_PATROL, getFraction(), color), Action::ActionType::WithWorldTile));
	vecActions.push_back(Action(id, (EventID)GeneralActions::StandGround, g_pRessourceManager->getIcon(PUD_ICON_HUMAN_STAND_GROUND, getFraction(), color)));
	return vecActions;
}


void Unit::doHandleDeath() {
	// Only decrease food for player-controlled units, not for neutral creatures like sheep
	if (getOwner() && getOwner() != getMap()->getNeutralPlayer()) {
		getOwner()->decreaseUsedFood();
	}
	if (isOrganic() && !isUnDead() && isLandObject() && isPlacedOnMap()) {
		getMap()->addAnimation(getTile(),
			g_pRessourceManager->getCorpseAnimation(this, m_facingDirection));
	}
}


void Unit::setIdleAnimation(Direction direction) {
	m_facingDirection = direction;
	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, direction);
}


Unit::~Unit() {
	std::cerr << "Unit::~Unit()" << std::endl;

	std::cerr << "\tUnit::~Unit() m_pIdle" << std::endl;
	delete m_pIdle;
	std::cerr << "\tUnit::~Unit() m_pMoving" << std::endl;
	delete m_pMoving;
	delete m_pPatrol;
	std::cerr << "\tUnit::~Unit() m_pDead" << std::endl;
	delete m_pDead;
}


void Unit::process() {
	m_currentAnimation.process();
	m_stateMachine.process();
	updateCombat();

	// Update buffs
	for (auto& pair : m_activeBuffs) {
		pair.second.process();
	}

	for (auto it = m_activeBuffs.begin(); it != m_activeBuffs.end(); ) {
		if (it->second.isExpired()) {
			it = m_activeBuffs.erase(it);
		}
		else {
			++it;
		}
	}
}

Object* Unit::selectAutomaticTarget(IArmed* armed) const {
	const std::set<Object*> targets = armed->findTargetObject(const_cast<Unit*>(this));
	if (targets.empty()) return nullptr;
	return *std::min_element(targets.begin(), targets.end(), [this](Object* lhs, Object* rhs) {
		auto distance = [this](Object* object) {
			const Point a = getTileLocation();
			const Point b = object->getTileLocation();
			return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
		};
		const int lhsDistance = distance(lhs);
		const int rhsDistance = distance(rhs);
		if (lhsDistance != rhsDistance) return lhsDistance < rhsDistance;
		return dynamic_cast<EventProcessor*>(lhs)->getID() <
			dynamic_cast<EventProcessor*>(rhs)->getID();
	});
}

void Unit::updateCombat() {
	IArmed* armed = dynamic_cast<IArmed*>(this);
	// Siege engines own a wind-up/release controller with different timing.
	if (!armed || !armed->isArmed() || dynamic_cast<Ballista*>(this) ||
		dynamic_cast<Dwarfes*>(this) ||
		getOwner()->isNeutral() ||
		isDead() || !isPlacedOnMap())
		return;
	if (m_attackCooldownRemaining > 0) {
		if (--m_attackCooldownRemaining == 0)
			setIdleAnimation(m_attackDirection);
		return;
	}
	if (isMoving())
		return;

	Object* target = armed->activeTarget();
	// Workers may defend themselves when explicitly ordered, but never acquire
	// targets autonomously while idle.
	if (!target && !dynamic_cast<Peasant*>(this) && isIdle()) {
		target = selectAutomaticTarget(armed);
		if (target) armed->assignTarget(target);
	}
	if (!target)
		return;
	if (!armed->activeTargetInRange(this)) {
		if (m_standGround) {
			armed->clearActiveTarget();
			return;
		}
		// An explicitly selected target may have moved while we approached it.
		if (isIdle()) {
			const Point destination = target->getTileLocation();
			handleEvent(Event(getID(), GeneralActions::Attack,
				destination.x, destination.y));
		}
		return;
	}

	const Point ownPosition = getTileLocation();
	const Point targetPosition = target->getTileLocation();
	m_attackDirection = ownPosition == targetPosition ? Direction::N :
		getTargetCardinalDirection(ownPosition, targetPosition);
	m_facingDirection = m_attackDirection;
	setCurrentAnimation(g_pRessourceManager->getUnitAnimation(
		this, m_attackDirection,
		ResourceManager_wartools::AnimationType::Attacking).playOnce());
	removeBuff(BuffType::Invisibility);
	if (armed->isCloseRangeUnit()) {
		AttackerStats damage = armed->getAttackerStats();
		if (hasBuff(BuffType::Bloodlust)) {
			damage.setBasicDamage(damage.getBasicDamage() * 2);
			damage.setPiercingDamage(damage.getPiercingDamage() * 2);
		}
		target->onAttackedBy(this);
		target->takeDamage(damage);
	} else {
		getMap()->addMapObject(new Projectile(
			ResourceManager_wartools::MissileType::Arrow, this, getMap(), target));
	}
	const int configured = armed->attackCooldownTicks();
	m_attackCooldownRemaining = configured > 0 ? configured :
		calculateRequiredTicksForGameAction(1000);
	if (hasBuff(BuffType::Slow))
		m_attackCooldownRemaining *= 2;
	else if (hasBuff(BuffType::Haste) && dynamic_cast<GryphonRider*>(this))
		m_attackCooldownRemaining = std::max(1, m_attackCooldownRemaining / 2);
}


void Unit::takeDamage(const AttackerStats& stats, bool noRandomComponent) {
	if (m_activeBuffs.contains(BuffType::UnholyArmor)) {
		return;
	}

	Object::takeDamage(stats, noRandomComponent);
}



void Unit::playRandomSound(const std::vector<std::string>& sounds) {
	if (sounds.empty())
		return;

	if (sounds.size() == 1) {
		g_pSoundManager->playSelectionWAV(sounds.front());
	}
	else {
		int randomIndex = PseudoRandom::cosmeticRnd(0, (int)sounds.size() - 1);
		g_pSoundManager->playSelectionWAV(sounds[randomIndex]);
	}
}


void Unit::playSoundSelected() {
	playRandomSound(m_vecSoundsSelected);
}






#include "Global/ErrorHandler.h"

namespace {
    void loadSoundsFromDirectory(const std::string& dirPath, std::vector<std::string>& soundVec,
                                const std::string& baseFolder) {
        if (!std::filesystem::exists(dirPath)) {
            return; // Some units don't have all sound types
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".wav") {
                    soundVec.push_back(baseFolder + entry.path().filename().string());
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            War25::Error::ErrorHandler::logWarning("Failed to load sounds from " + dirPath + ": " + e.what());
        }
    }
}

void Unit::setSoundBaseDirectory(const std::string& strPathToSoundFolder) {
	// Some units switch sounds when upgrade, e.g. Knight and Paladin. Hence, we clean the vectors when this method is called
	m_vecSoundsSelected.clear();
	m_vecSoundsAnnoyed.clear();
	m_vecSoundsAcknowledge.clear();

	// Load sounds using the new helper function
	const std::string fullBasePath = SoundManager::strFolderPrefix + strPathToSoundFolder;

	loadSoundsFromDirectory(fullBasePath + "selected/", m_vecSoundsSelected, strPathToSoundFolder + "selected/");
	loadSoundsFromDirectory(fullBasePath + "annoyed/", m_vecSoundsAnnoyed, strPathToSoundFolder + "annoyed/");
	loadSoundsFromDirectory(fullBasePath + "acknowledgement/", m_vecSoundsAcknowledge, strPathToSoundFolder + "acknowledgement/");
}
