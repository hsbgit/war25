#include "resourcemanager_wartools.h"
#include "ResourceMappings.h"
#include "Sprite.h"

#include <stdexcept>

void ResourceManager_wartools::switchBuildingSprites(Season s) {
	// Okay, this needs a bit of explanation. Wasteland actually uses many of the
	// summer sprites. As we just copy the pointers / color maps to the 'm_ActiveSeasonBuildingsHuman' (and orc) container,
	// we copy summer first and the few individual wasteland keys/values are replacing the some of the summer sprites afterwards
	if (s == Season::Wasteland)
		switchBuildingSprites(Season::Summer);

	// Sprites keep Texture** pointers to entries in these active maps. Replacing
	// an entire unordered_map destroys its nodes and leaves every existing
	// building with dangling pointers. Update values in place so their addresses
	// remain stable across season changes.
	auto updateColors = [](auto& target, const auto& source) {
		for (const auto& [color, texture] : source)
			target[color] = texture;
	};
	auto updateUpgradeLevels = [&updateColors](auto& target, const auto& source) {
		for (const auto& [level, colors] : source)
			updateColors(target[level], colors);
	};

	for (const auto& [buildingType, seasonMap] : m_mapBuildingTexturesHuman) {
		if (seasonMap.contains(s))
			updateColors(m_ActiveSeasonBuildingsHuman[buildingType], seasonMap.at(s));
	}

	for (const auto& [buildingType, seasonMap] : m_mapBuildingTexturesOrc) {
		if (seasonMap.contains(s))
			updateColors(m_ActiveSeasonBuildingsOrc[buildingType], seasonMap.at(s));
	}


	// Townhall and ScoutTower are special, as 3 different *.pngs map to the same typeid -> Townhall and ScoutTower
	// Thus, they have different structured maps.
	if (m_mapTownhallTexturesHuman.contains(s))
		updateUpgradeLevels(m_ActiveSeasonTownhallHuman, m_mapTownhallTexturesHuman.at(s));

	if (m_mapTownhallTexturesOrc.contains(s))
		updateUpgradeLevels(m_ActiveSeasonTownhallOrc, m_mapTownhallTexturesOrc.at(s));

	if (m_mapScoutTowerTexturesHuman.contains(s))
		updateUpgradeLevels(m_ActiveSeasonScoutTowerTexturesHuman, m_mapScoutTowerTexturesHuman.at(s));

	if (m_mapScoutTowerTexturesOrc.contains(s))
		updateUpgradeLevels(m_ActiveSeasonScoutTowerTexturesOrc, m_mapScoutTowerTexturesOrc.at(s));
}


void ResourceManager_wartools::switchSeason(Season season) {
	if (!m_mapTileSets.contains(season) || !m_mapCritters.contains(season) ||
		!m_SeasonIcons.contains(season) ||
		!m_mapDestroyedSites.contains(BuildingDestroyedType::DestroyedBig) ||
		!m_mapDestroyedSites.at(BuildingDestroyedType::DestroyedBig).contains(season))
		throw std::invalid_argument("Requested season has not been loaded");
	m_currentSeason = season;
	switchSeasonIcons(season);
	switchBuildingSprites(season);
	m_pActiveTileSet = m_mapTileSets.at(season);
	m_pActiveCritterTexture = m_mapCritters.at(season);


	// Refresh tile sprites, each Tile object has a pointer
	// to one of the stored sprites
	for (auto& [tile_id, sprite] : m_tileLookUp) {
		const auto mapping = m_mapPUDId_to_TileSetIndex.find(tile_id);
		if (mapping == m_mapPUDId_to_TileSetIndex.end() || !mapping->second.contains(season))
			continue;
		int val = mapping->second.at(season);
		int rowOffset = val / 16;
		int colOffset = val % 16;

		// startoffset
		int iconWidth_px = 32;
		int iconHeight_px = 32;

		Rect rect = { colOffset * iconWidth_px, rowOffset * iconHeight_px, iconWidth_px, iconHeight_px };
		sprite = Sprite(&m_pActiveTileSet, rect);
	}

	m_animDestroyedBuilding.setBaseTexture(m_mapDestroyedSites.at(BuildingDestroyedType::DestroyedBig).at(m_currentSeason));
	m_animDestroyedBuildingSea.setBaseTexture(m_mapDestroyedSites.at(BuildingDestroyedType::DestroyedBig).at(m_currentSeason));

	notifySeasonObservers();
}


void ResourceManager_wartools::switchSeasonIcons(Season s) {
	for (auto const& [color, palette] : mapPlayerPaletteColors) {
		m_ActiveSeasonIcons[color] = m_SeasonIcons.at(s).at(color);
	}
}
