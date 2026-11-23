#include "CircleOfPower.h"

#include "Resources/resourcemanager_wartools.h"

CircleOfPower::CircleOfPower(Player* owner, Map* map)
	: Building(owner, Fraction::Neutral, map, 1, 0, 0) {
	m_icon = g_pRessourceManager->getIcon(PUD_ICON_CIRCLE_OF_POWER);
	m_sprite = g_pRessourceManager->getNeutralBuildingSprite(this);
	setName("Circle of Power");
}
