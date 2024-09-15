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
#include "Resources/SoundManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "EventHandling/StateCondition.h"
#include "EventHandling/StateIdle.h"
#include "EventHandling/StateMoving.h"
#include "EventHandling/StateDead.h"
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
	m_pMoving = new StateMoving(this); // ToDo: Keep track and delete them when object is destroyed!
	m_pPatrol = new CompositeStatePatrol(this);
	m_pDead = new StateDead(this);

	m_pIdle->addTransition(EventID(GeneralActions::Move), m_pMoving);
	m_pIdle->addTransition(EventID(GeneralActions::Patrol), m_pPatrol);

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




	m_pMoving->addTransition(EventID(GeneralActions::Attack), m_pIdle);
	m_pMoving->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pMoving->addTransition(EventID(GeneralActions::Move), m_pMoving); // If a unit is already moving and another move event occurs


	m_pPatrol->addTransition(EventID(GeneralActions::Stop), m_pIdle);
	m_pPatrol->addTransition(EventID(GeneralActions::Move), m_pMoving);


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


void Unit::handleEvent(const Event& event) {
	std::cerr << "Unit: Received Event ID: " << std::dec << (int)event.getEventID() << std::endl;
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
	getOwner()->decreaseUsedFood();
	// ToDo: Stop all actions / states 
}


void Unit::setIdleAnimation() {
	m_currentAnimation = g_pRessourceManager->getUnitIdleAnimation(this, Direction::N);
}


Unit::~Unit() {
	std::cerr << "Unit::~Unit()" << std::endl;

	std::cerr << "\tUnit::~Unit() m_pIdle" << std::endl;
	delete m_pIdle;
	std::cerr << "\tUnit::~Unit() m_pMoving" << std::endl;
	delete m_pMoving;
	std::cerr << "\tUnit::~Unit() m_pDead" << std::endl;
	delete m_pDead;
}


void Unit::process() {
	m_currentAnimation.process();
	m_stateMachine.process();

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
		g_pSoundManager->playWAV(sounds.front());
	}
	else {
		int randomIndex = PseudoRandom::rnd(0, (int)sounds.size() - 1);
		g_pSoundManager->playWAV(sounds[randomIndex]);
	}
}


void Unit::playSoundSelected() {
	playRandomSound(m_vecSoundsSelected);
}



void Unit::playSoundAcknowledge() {
	playRandomSound(m_vecSoundsAcknowledge);
}



// todo: split body in 1 method
void Unit::setSoundBaseDirectory(const std::string& strPathToSoundFolder) {
	namespace fs = std::filesystem;

	// Some units switch sounds when upgrade, e.g. Knight and Paladin. Hence, we clean the vectors when this method is called
	m_vecSoundsSelected.clear();
	m_vecSoundsAnnoyed.clear();
	m_vecSoundsAcknowledge.clear();

	// Some units do not have selected sounds, e.g., the gnomish flying machine
	if (std::filesystem::exists(SoundManager::strFolderPrefix + strPathToSoundFolder + "selected/")) {
		for (const auto& entry : fs::directory_iterator(SoundManager::strFolderPrefix + strPathToSoundFolder + "selected/")) {
			if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".wav") {
				m_vecSoundsSelected.push_back(strPathToSoundFolder + "selected/" + entry.path().filename().string());
			}
		}
	}

	// Some units do not have annoyed sounds, e.g., the gryphon rider
	if (std::filesystem::exists(SoundManager::strFolderPrefix + strPathToSoundFolder + "annoyed/")) {
		for (const auto& entry : fs::directory_iterator(SoundManager::strFolderPrefix + strPathToSoundFolder + "annoyed/")) {
			if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".wav") {
				m_vecSoundsAnnoyed.push_back(strPathToSoundFolder + "annoyed/" + entry.path().filename().string());
			}
		}
	}


	if (std::filesystem::exists(SoundManager::strFolderPrefix + strPathToSoundFolder + "acknowledgement/")) {
		for (const auto& entry : fs::directory_iterator(SoundManager::strFolderPrefix + strPathToSoundFolder + "acknowledgement/")) {
			if (entry.is_regular_file() && fs::path(entry.path()).extension() == ".wav") {
				m_vecSoundsAcknowledge.push_back(strPathToSoundFolder + "acknowledgement/" + entry.path().filename().string());
			}
		}
	}
}
