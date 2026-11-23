#pragma once

#include "Building.h"

class Runestone final : public Building {
public:
	Runestone(Player* owner, Map* map);
};
