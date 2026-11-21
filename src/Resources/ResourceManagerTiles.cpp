#include "resourcemanager_wartools.h"
#include "Texture.h"
#include "Sprite.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace { constexpr int TileOffsetIncrement = 0x010; }

void ResourceManager_wartools::parseLUATileSetFile(Season season, const std::string& strFile) {
	std::ifstream file(strFile);

	if (!file) {
		throw std::runtime_error("Unable to open LUA tileset file: " + strFile);
	}


	int tile_id = 0x000;
	std::string s;
	while (std::getline(file, s))
	{
		// Second condition avoids rows which have "--" but do not represent tile id values (look at file...)
		if ((s.find("--") != std::string::npos) && (s.substr(0, 4) == "    ")) {
			s.erase(s.find("--"));
			s.erase(std::remove(s.begin(), s.end(), '{'), s.end());
			s.erase(std::remove(s.begin(), s.end(), '}'), s.end());
			std::erase_if(s, [](unsigned char character) { return std::isspace(character); });
			while (!s.empty() && s.back() == ',') s.pop_back();

			if (s.size()) {
				std::string entry;
				std::stringstream ss(s);

				int sub_tile_id = tile_id;
				while (getline(ss, entry, ',')) {
					if (entry.empty()) {
						++sub_tile_id;
						continue;
					}
					try {
						int index = std::stoi(entry);

						if (index != 0) {
							m_mapPUDId_to_TileSetIndex[sub_tile_id][season] = index;
						}
					}
					catch (const std::exception& e) {
						std::cerr << "Warning: Invalid tile index in " << strFile << ": " << entry << " (" << e.what() << ")" << std::endl;
					}

					++sub_tile_id;
				}
			}

			tile_id += TileOffsetIncrement;
		}
	}

	m_mapPUDId_to_TileSetIndex[SpecialTile::RemovedWood][season] = 126; // How to get this: Look at *.png and cnt rows and columns: 7*16+14 = 126
	m_mapPUDId_to_TileSetIndex[SpecialTile::RemovedRock][season] = 10 * 16 + 6;

	// The tileset exposes one canonical rubble tile for destroyed walls.
	m_mapPUDId_to_TileSetIndex[SpecialTile::DestroyedWalls][season] = 5 * 16 + 8;


}


Sprite* ResourceManager_wartools::getTileSprite(int tile_id) {
	assert(m_pActiveTileSet); // Required tileset texture available?

	// tile_id and corresponding season exist?
	if (!m_mapPUDId_to_TileSetIndex.contains(tile_id) ||
		!m_mapPUDId_to_TileSetIndex.at(tile_id).contains(m_currentSeason)) {
		// Custom maps occasionally use an empty/invalid tile slot. Keep the map
		// loadable and display the first tileset cell as a visible placeholder.
		std::cerr << "Warning: tile id 0x" << std::hex << tile_id
			<< " has no mapping for tileset " << std::dec << static_cast<int>(m_currentSeason)
			<< "; using placeholder" << std::endl;
		m_mapPUDId_to_TileSetIndex[tile_id][m_currentSeason] = 0;
	}

	// Just create one sprite for a tile_id, not several!
	if (!m_tileLookUp.contains(tile_id)) {
		int val = m_mapPUDId_to_TileSetIndex[tile_id][m_currentSeason];
		int rowOffset = val / 16;
		int colOffset = val % 16;

		int iconWidth_px = 32;
		int iconHeight_px = 32;
		Rect rect = { colOffset * iconWidth_px, rowOffset * iconHeight_px, iconWidth_px, iconHeight_px };

		m_tileLookUp.insert_or_assign(tile_id, Sprite(&m_pActiveTileSet, rect));
	}

	return &m_tileLookUp.at(tile_id);
}


void ResourceManager_wartools::readCritters() {
	m_mapCritters[Season::Summer] = makeTexture("data/graphics/tilesets/summer/neutral/units/critter.png", m_pRenderer);
	m_mapCritters[Season::Swamp] = makeTexture("data/graphics/tilesets/swamp/neutral/units/critter.png", m_pRenderer);
	m_mapCritters[Season::Winter] = makeTexture("data/graphics/tilesets/winter/neutral/units/critter.png", m_pRenderer);
	m_mapCritters[Season::Wasteland] = makeTexture("data/graphics/tilesets/wasteland/neutral/units/critter.png", m_pRenderer);
}


void ResourceManager_wartools::readTileSetTextures() {
	m_mapTileSets[Season::Summer] = makeTexture("data/graphics/tilesets/summer/terrain/summer.png", m_pRenderer);
	m_mapTileSets[Season::Swamp] = makeTexture("data/graphics/tilesets/swamp/terrain/swamp.png", m_pRenderer);
	m_mapTileSets[Season::Winter] = makeTexture("data/graphics/tilesets/winter/terrain/winter.png", m_pRenderer);
	m_mapTileSets[Season::Wasteland] = makeTexture("data/graphics/tilesets/wasteland/terrain/wasteland.png", m_pRenderer);
}
