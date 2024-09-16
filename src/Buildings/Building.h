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
#include <cassert>

class Peasant;

#include "EventHandling/BuildingEventProcessor.h"


// Sight: tower 9, townhall 4, all other 3
class Building : public Object, public BuildingEventProcessor {
public:
	enum class BuildingState { InConstruction1, InConstruction2, InConstruction3, Constructed, SlightlyDamaged, HeavilyDamaged, Destroyed };

	Building(Peasant* pErecter, Map* pMap, int constructionTime, int healthPointsMax, int sight, int armor = 20);
	Building(Player* pOwner, Fraction fraction, Map* pMap, int healthPointsMax, int sight, int armor = 20);
	~Building();

	inline BuildingState getBuildingState() const {
		return m_currState;
	}

	void process() override;

	int getSight() const override {
		return (m_currState < BuildingState::Constructed) ? 1 : Object::getSight();
	}

	virtual Type getType() const override {
		return Type::Land;
	}

	inline const Sprite& sprite() const override {
		assert(m_sprite.getTexture());
		return m_sprite;
	}

	bool isMoving() const override {
		return false;
	}

	void doHandleDeath() override {
		// Empty on purpose
	}

	std::vector<Action> getActions() const override;


	inline void setSprite(const Sprite& sprite) {
		m_sprite = sprite;
	}

	inline bool isUnderConstruction() const {
		return (m_currState == BuildingState::InConstruction1 || m_currState == BuildingState::InConstruction2 || m_currState == BuildingState::InConstruction3);
	}

protected:
	void playSoundSelected() override;

	inline void setSoundSelected(const std::string& strPath) {
		m_strSoundSelected = strPath;
	}

	inline bool isSmallBuilding() {
		return !(getOccupiedTiles().size() > 4);
	}

	virtual std::vector<ProductionItem::ItemType> getProductionItemList() const {
		return std::vector<ProductionItem::ItemType>();
	}

	BuildingState m_currState = BuildingState::Constructed;

	Sprite m_sprite;

	// Called when a building was created with the Peasant constructor and the construction has finished
	virtual void onConstructionFinished();

private:
	Peasant* m_pErecter = nullptr; // Only set in the ctor with the constructionTime. In this case m_pErector is the Peasant building. He must be placed on the map when the construction is finished, see 'onConstructionFinished()' 
	void handleBuildProgress();
	int m_construction_ticksPerHitpoint = -1; // When built: How many ticks are required to add 1 health to the build progress? Only set if required, in the constructor with the peasant
	int m_tickCounter = 1;

	std::string m_strSoundSelected;
};

