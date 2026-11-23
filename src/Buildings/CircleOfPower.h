#pragma once

#include "Building.h"

class CircleOfPower final : public Building {
public:
	CircleOfPower(Player* owner, Map* map);
	void takeDamage(const AttackerStats&, bool = false) override {}
	bool isWalkableGroundOverlay() const override { return true; }
};
