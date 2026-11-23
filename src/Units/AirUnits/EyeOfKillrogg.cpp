#include "EyeOfKillrogg.h"

#include "Global/GlobalAttributes.h"
#include "Player.h"
#include "Resources/resourcemanager_wartools.h"

EyeOfKillrogg::EyeOfKillrogg(Player* owner, Map* map)
	: Unit(owner, Fraction::Orc, map, 100, 0, 3, 42, false),
	  m_remainingTicks(calculateRequiredTicksForGameAction(18000)) {
	setName("Eye of Killrogg");
	setDyingSound("misc/explosion.wav");
	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(
		PUD_ICON_EYE_OF_KILROGG, getFraction(), getOwner()->getColor());
}

void EyeOfKillrogg::process() {
	Unit::process();
	if (!isDead() && --m_remainingTicks <= 0)
		die();
}
