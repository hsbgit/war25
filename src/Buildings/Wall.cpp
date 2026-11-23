#include "Wall.h"

#include "Player.h"
#include "Resources/resourcemanager_wartools.h"

Wall::Wall(Player* owner, Fraction fraction, Map* map)
	: Building(owner, fraction, map, 40, 1, 20) {
	m_icon = g_pRessourceManager->getIcon(
		fraction == Fraction::Alliance ? PUD_ICON_HUMAN_WALL : PUD_ICON_ORC_WALL,
		owner->getColor());
	m_sprite = g_pRessourceManager->getNeutralBuildingSprite(this);
	setName("Wall");
}
