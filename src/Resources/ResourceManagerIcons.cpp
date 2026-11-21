#include "resourcemanager_wartools.h"
#include "ResourceMappings.h"
#include "Texture.h"
#include "Sprite.h"

#include <cassert>

void ResourceManager_wartools::readIcons() {
	std::string strToFolder("data/graphics/tilesets/");
	SDL_Surface* pSurface_Icons_Summer = IMG_Load((strToFolder + "summer/icons.png").c_str());
	SDL_Surface* pSurface_Icons_Swamp = IMG_Load((strToFolder + "swamp/icons.png").c_str());
	SDL_Surface* pSurface_Icons_Winter = IMG_Load((strToFolder + "winter/icons.png").c_str());
	SDL_Surface* pSurface_Icons_Wasteland = IMG_Load((strToFolder + "wasteland/icons.png").c_str());

	assert(pSurface_Icons_Summer);
	assert(pSurface_Icons_Swamp);
	assert(pSurface_Icons_Winter);
	assert(pSurface_Icons_Wasteland);

	for (auto const& [color, palette] : mapPlayerPaletteColors) {
		SDL_SetPaletteColors(pSurface_Icons_Summer->format->palette, &palette[0], 208, 4);
		SDL_SetPaletteColors(pSurface_Icons_Swamp->format->palette, &palette[0], 208, 4);
		SDL_SetPaletteColors(pSurface_Icons_Winter->format->palette, &palette[0], 208, 4);
		SDL_SetPaletteColors(pSurface_Icons_Wasteland->format->palette, &palette[0], 208, 4);

		m_SeasonIcons[Season::Summer][color] = makeTexture(pSurface_Icons_Summer, m_pRenderer, false);
		m_SeasonIcons[Season::Swamp][color] = makeTexture(pSurface_Icons_Swamp, m_pRenderer, false);
		m_SeasonIcons[Season::Winter][color] = makeTexture(pSurface_Icons_Winter, m_pRenderer, false);
		m_SeasonIcons[Season::Wasteland][color] = makeTexture(pSurface_Icons_Wasteland, m_pRenderer, false);
	}

	SDL_FreeSurface(pSurface_Icons_Summer);
	SDL_FreeSurface(pSurface_Icons_Swamp);
	SDL_FreeSurface(pSurface_Icons_Winter);
	SDL_FreeSurface(pSurface_Icons_Wasteland);
}




/*
 * Parameter fraction is only of interest if we, e.g., search for "PUD_ICON_MAGE" which
 * has id 14. All orc units of the same type follow, this means the Death Knight "PUD_ICON_DEATH_KNIGHT"
 * has id 15. Thus, if fraction==orc we simply add 1 to the icon index.
 */
Sprite ResourceManager_wartools::getIcon(Pud_Icon icon, Fraction fraction, PlayerColor color) {
	/*
		PUD_ICON_SWORD_0
	    PUD_ICON_HUMAN_ARR
	    PUD_ICON_HUMAN_SHIELD_0
   */

	if (fraction == Fraction::Alliance)
		return getIcon(icon, color);

	// These are shared submenu buttons, not human/orc icon pairs.
	if (icon == PUD_ICON_BUILD_STAGE_1 || icon == PUD_ICON_BUILD_STAGE_2 ||
		icon == PUD_ICON_CANCEL)
		return getIcon(icon, color);

	// It is an Orc Icon:    
	// Special cases because we have icons that change during upgrades. Hence, we cant increment as the ordering is, e.g., human sword 0,1,2 and then axe 0,1,2
	if (icon == PUD_ICON_SWORD_0)
		icon = PUD_ICON_AXE_0;
	else if (icon == PUD_ICON_HUMAN_SHIELD_0)
		icon = PUD_ICON_ORC_SHIELD_0;
	else {
		// Unfortunately operator++ is not supported for enums...
		int pos = static_cast<int>(icon) + 1;

		// Special case...
		if (icon == PUD_ICON_HUMAN_GUARD_TOWER || icon == PUD_ICON_HUMAN_CANNON_TOWER) {
			++pos;
		}

		icon = static_cast<Pud_Icon>(pos);
	}

	return getIcon(icon, color);
}


Sprite ResourceManager_wartools::getIcon(Pud_Icon icon, PlayerColor color) {
	int columns = 5; //, rows = 38;
	int iconWidth_px = 46;
	int iconHeight_px = 38;

	int pos = static_cast<int>(icon);
	int nCols = pos % columns; // Get column id
	int nRows = pos / columns; // Get row id

	Rect rect = { nCols * iconWidth_px, nRows * iconHeight_px, iconWidth_px, iconHeight_px };

	if (color == PlayerColor::Neutral) {
		// Neutral Icons do not have a color palette (red, blue, etc.)
		// As it does not matter, simply take the icon texture colored for PlayerColor::Red
		color = PlayerColor::Red;
	}

	return Sprite(&m_ActiveSeasonIcons[color], rect);
}


