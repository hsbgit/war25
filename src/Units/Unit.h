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

#include "Global/Object.h"
#include "Resources/Animation.h"
#include "EventHandling/EventProcessor.h"
#include "EventHandling/StateMachine.h"
#include "Magic/Buff.h"
#include "UnitActions.h"

class Path;

/*
 * Some thoughts:
 *
 * Critter : move (no animation)  Object Unit
 * Tower   : non-move, fight      Object Building ArmedBuilding
 * Footman : move, fight          Object Unit ArmedUnit Footman
 * Zeppelin: move, non-fight      Object Unit
 *
 * Hence, we separately implement interface IArmed for units that can fight
 */
class Unit : public Object, public EventProcessor {
public:
	Unit(Player* pOwner, Fraction fraction, Map* pMap, int healthPointsMax, int armor, int sight, int speed);
	virtual ~Unit();

	inline int getSpeed() const {
		return m_speed;
	}

	virtual bool isOrganic() const {
		return true;
	}

	virtual bool isUnDead() const {
		return false;
	}


	// Object
	void process() override;
	bool isMoving() const override;
	std::vector<Action> getActions() const override;


	void takeDamage(const AttackerStats& stats, bool noRandomComponent = false) override;

	void handleEvent(const Event& event) override;

	void doHandleDeath() override;


	inline const Sprite& sprite() const override {
		assert(m_currentAnimation.getCurrentFrame().getTexture());
		return m_currentAnimation.getCurrentFrame();
	}

	inline void setCurrentAnimation(const Animation& animation) {
		m_currentAnimation = animation;
	}

	void setIdleAnimation();

	void addBuff(const Buff& buff) {
		auto it = m_activeBuffs.find(buff.getType());
		if (it != m_activeBuffs.end()) {
			it->second.reset();
		}
		else {
			m_activeBuffs[buff.getType()] = buff;
		}
	}

	inline bool hasActiveBuffs() const {
		return m_activeBuffs.size();
	}


	inline const std::map<BuffType, Buff>& getActiveBuffs() {
		return m_activeBuffs;
	}


	// ToDo: Information hiding
	std::map<BuffType, Buff> m_activeBuffs;


	State* m_pIdle = nullptr;
	State* m_pMoving = nullptr;
	State* m_pPatrol = nullptr;
	State* m_pDead = nullptr;

protected:
	StateMachine m_stateMachine;

protected:
	// Object
	void playSoundSelected() override;
	void playSoundAcknowledge() override;
	void setSoundBaseDirectory(const std::string& strPathToSoundFolder);
	std::vector<std::string> m_vecSoundsSelected, m_vecSoundsAnnoyed, m_vecSoundsAcknowledge;

	Animation m_currentAnimation;

private:
	void playRandomSound(const std::vector<std::string>& sounds);
	int m_speed;
};


