#include "resourcemanager_wartools.h"

#include <stdexcept>

void ResourceManager_wartools::refreshLoadedCursor() {
	if (m_pLastSetCursor) {
		SDL_SetCursor(m_pLastSetCursor);
	}
}


void ResourceManager_wartools::setCursor(CursorTypes type) {
	assert(m_cursors.contains(type));
	SDL_SetCursor(m_cursors[type]);
	m_pLastSetCursor = m_cursors[type];
}


void ResourceManager_wartools::loadCursors() {
	// Human Cursors
	std::string strHumanPath = strPrefix + "ui" + PATH_SEPARATOR + "human" + PATH_SEPARATOR + "cursors" + PATH_SEPARATOR;

	SDL_Surface* pSurface = IMG_Load((strHumanPath + "red_eagle.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strHumanPath + "red_eagle.png");
	}
	m_cursors[CursorTypes::RedEagle] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);


	pSurface = IMG_Load((strHumanPath + "yellow_eagle.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strHumanPath + "yellow_eagle.png");
	}
	m_cursors[CursorTypes::YellowEagle] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);

	pSurface = IMG_Load((strHumanPath + "green_eagle.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strHumanPath + "green_eagle.png");
	}
	m_cursors[CursorTypes::GreenEagle] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);

	pSurface = IMG_Load((strHumanPath + "human_gauntlet.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strHumanPath + "human_gauntlet.png");
	}
	m_cursors[CursorTypes::HumanGauntlet] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);



	// Orc Cursors
	std::string strOrcPath = strPrefix + "ui" + PATH_SEPARATOR + "orc" + PATH_SEPARATOR + "cursors" + PATH_SEPARATOR;

	pSurface = IMG_Load((strOrcPath + "red_crosshairs.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strOrcPath + "red_crosshairs.png");
	}
	m_cursors[CursorTypes::RedCrosshairs] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);

	pSurface = IMG_Load((strOrcPath + "yellow_crosshairs.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strOrcPath + "yellow_crosshairs.png");
	}
	m_cursors[CursorTypes::YellowCrosshairs] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);

	pSurface = IMG_Load((strOrcPath + "green_crosshairs.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strOrcPath + "green_crosshairs.png");
	}
	m_cursors[CursorTypes::GreenCrosshairs] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);

	pSurface = IMG_Load((strOrcPath + "orcish_claw.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: " + strOrcPath + "orcish_claw.png");
	}
	m_cursors[CursorTypes::OrcishClaw] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);

	// Neutral
	pSurface = IMG_Load((strPrefix + "ui" + PATH_SEPARATOR + "cursors" + PATH_SEPARATOR + "magnifying_glass.png").c_str());
	if (!pSurface) {
		throw std::runtime_error("Failed to load cursor texture: magnifying_glass.png");
	}
	m_cursors[CursorTypes::MagnifyingGlass] = SDL_CreateColorCursor(pSurface, 0, 0);
	SDL_FreeSurface(pSurface);
}




