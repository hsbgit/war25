#pragma once

#include "Building.h"

class Wall final : public Building {
public:
	Wall(Player* owner, Fraction fraction, Map* map);
};
