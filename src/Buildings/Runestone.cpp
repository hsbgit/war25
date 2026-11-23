#include "Runestone.h"

#include "Resources/resourcemanager_wartools.h"

Runestone::Runestone(Player* owner, Map* map)
	: Building(owner, Fraction::Neutral, map, 5000, 4, 20) {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_RUNESTONE);
	setAmbientAnimation(g_pRessourceManager->getNeutralBuildingAnimation(this));
	setName("Runestone");
}
