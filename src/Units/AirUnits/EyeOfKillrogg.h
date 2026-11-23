#pragma once

#include "Units/Unit.h"

// Temporary, player-controlled scout summoned by an Ogre Mage.  It has its
// own type because the Wargus eye_of_kilrogg.png sheet is 5x32 pixels and must
// never share Ballista's texture/animation cache key.
class EyeOfKillrogg final : public Unit {
public:
	EyeOfKillrogg(Player* owner, Map* map);

	void process() override;
	bool isOrganic() const override { return false; }
	Type getType() const override { return Type::Air; }

private:
	int m_remainingTicks;
};
