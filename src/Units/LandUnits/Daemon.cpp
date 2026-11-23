#include "Daemon.h"

#include "Player.h"
#include "Resources/resourcemanager_wartools.h"

Daemon::Daemon(Player* owner, Map* map)
	: Unit(owner, owner->getFraction(), map, 60, 2, 5, 14, false),
	  IArmed(9, 1, 2) {
	setName("Daemon");
	setDyingSound("orc/basic_voices/dead.wav");
	setIdleAnimation();
	m_icon = g_pRessourceManager->getIcon(
		PUD_ICON_DAEMON, Fraction::Orc, getOwner()->getColor());
}
