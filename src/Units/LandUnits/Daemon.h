#pragma once

#include "Units/Unit.h"

class Daemon final : public Unit, public IArmed {
public:
	Daemon(Player* owner, Map* map);

	Type getType() const override { return Type::Land; }
	int getAdditionalPiercingDamage() const override { return 0; }
	int getAdditionalAttackRange() const override { return 0; }
};
