/*
    Copyright (C) 2024 P. Last

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "BuildingEventProcessor.h"
#include "EventBroker.h"

#include "Player.h"
#include "Map/Map.h"
#include "Units/AirUnits/GnomishFlyingMachine.h"
#include "Units/AirUnits/GryphonRider.h"
#include "Units/SeaUnits/Battleship.h"
#include "Units/SeaUnits/Destroyer.h"
#include "Units/SeaUnits/GnomishSubmarine.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/SeaUnits/Transport.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Units/LandUnits/Dwarfes/Dwarfes.h"
#include "Units/LandUnits/Archer.h"
#include "Units/LandUnits/Ballista.h"
#include "Units/LandUnits/Footman.h"
#include "Units/LandUnits/Knight.h"
#include "Units/LandUnits/Mage.h"

#include "Global/Object.h"
#include "Global/TechnologyManager.h"
#include "Global/GlobalAttributes.h"
#include "Global/DebugManager.h"
#include "Resources/resourcemanager_wartools.h"
#include "Resources/SoundManager.h"
#include <iostream>


const std::size_t BuildingEventProcessor::maxEventQueueLength = 11; // 1 is always in production and up to 10 can be queued additionally

// @param buildingFraction
// The building fraction is important, not the fraction of the owner!
// In other words: Player can be human who owns an orc harbour
BuildingEventProcessor::BuildingEventProcessor(Object* pObject, Fraction buildingFraction, Map* pMap) : EventProcessor(), m_currEventInProgressIcon(g_pRessourceManager->getUISprite("orc//cursors//red_crosshairs.png")), m_pObject(pObject), m_pObjectOwner(m_pObject->getOwner()), m_pMap(pMap), m_buildingFraction(buildingFraction) {

}


BuildingEventProcessor::~BuildingEventProcessor() {
    // ToDo: cancel ongoing research stuff
}


void BuildingEventProcessor::handleEvent(const Event& event) {
    if(m_eventQueue.size() >= maxEventQueueLength)
        return;

    EventID eventID = event.getEventID();    
    ProductionItem::ItemType itemType = (ProductionItem::ItemType)eventID;    
    assert(gMapProductionItems.contains(itemType));

    if (itemType > ProductionItem::ItemType::LANDUNITS_BEGIN && itemType < ProductionItem::ItemType::UPGRADES_BEGIN) {
        // A unit production. Check if there is enough food
        if (!gDebugManager.m_ignoreFood && !getObject()->getOwner()->isFoodAvailable()) {
            g_pSoundManager->playErrorWAV();
#ifdef _DEBUG
            std::cerr << "Called EventProcessor::handleEvent! ERROR: Not enough food!" << std::endl;
#endif
            return;
        }
    }

    const ProductionItem& item = gMapProductionItems[itemType];

    if(!m_pObjectOwner->takeFromResources(item.costs())) {
        g_pSoundManager->playErrorWAV();
#ifdef _DEBUG
        std::cerr<<"Called EventProcessor::handleEvent! ERROR: Not enough resources!"<<std::endl;
#endif
        return;
    }


    if(item.isResearchItem()) {
        TechnologyManager* pTechManager = m_pObjectOwner->getTechnologyCoordinator();
        assert(!pTechManager->isResearched(itemType));
        pTechManager->markResearchAsInProgress(itemType); 
    }


    /*
     * Push all three containers.
     *
     * Object's fraction matters, not the owner! The actual owner may be a human, but
     * the object may be orc
     */    
    m_vecQueuedEventIcons.push_back(g_pRessourceManager->getIcon(item.getPUDIconID(getObject()->getFraction()), m_pObjectOwner->getColor()));
    m_vecQueuedProductionItems.push(item);
    m_eventQueue.push(event);
}


void BuildingEventProcessor::doEventHandling() {
    // Do not change from EventProcessor::isEventInProgress() to isEventInProgress()
    // This will break the Townhall when upgrading to keep or castle!
    if(!EventProcessor::isEventInProgress() && hasQueuedEvents()) {
        m_currEventInProgressIcon = m_vecQueuedEventIcons.front();
        m_currEventInProgressItem = m_vecQueuedProductionItems.front();

        m_vecQueuedEventIcons.erase(m_vecQueuedEventIcons.begin());
        m_vecQueuedProductionItems.pop();
        
        // Start research
        m_tickCnt = 0;
        m_requiredEventTicks = calculateRequiredTicksForGameAction( m_currEventInProgressItem.researchTime() );
        setEventInProgress(true);
    } else if(EventProcessor::isEventInProgress() && (++m_tickCnt == m_requiredEventTicks)) {
        if (eventFinished(m_eventQueue.front())) {
            m_eventQueue.pop();
            setEventInProgress(false);
        }
    }
}


bool BuildingEventProcessor::eventFinished(const Event& event) {
    ProductionItem::ItemType itemType = (ProductionItem::ItemType)event.getEventID();

    if (m_currEventInProgressItem.isResearchItem()) {
        TechnologyManager* pTechManager = m_pObjectOwner->getTechnologyCoordinator();
        pTechManager->researchFinished(itemType);
        return true;
    }
 
    return createUnit(itemType);
}


bool BuildingEventProcessor::createUnit(ProductionItem::ItemType unitType) {
    int tileSize = (unitType > ProductionItem::ItemType::LANDUNITS_END) ? 2 : 1;

    Object::Type landAirSea = getUnitDomain(unitType); 
    Tile* pTile = getObject()->getMap()->getFreeTileAroundBuilding(getObject(), tileSize, landAirSea);

    if (!pTile) {
#ifdef _DEBUG
        std::cerr << "No free tile to place unit. Waiting..." << std::endl;
#endif
        return false;
    }


    Object* pObject = createUnitObject(unitType);
    assert(pObject);
   

    // Equip with all so far researched upgrades
    TechnologyManager* pTechManager = m_pObjectOwner->getTechnologyCoordinator();
    pTechManager->installUpgrades(pObject);
    getObject()->getMap()->placeUnit(pTile, pObject);
    return true;
}


Object::Type BuildingEventProcessor::getUnitDomain(ProductionItem::ItemType unitType) {
    if (unitType > ProductionItem::ItemType::AIRUNITS_BEGIN && unitType < ProductionItem::ItemType::AIRUNITS_END) {
        return Object::Type::Air;
    }
    else if (unitType > ProductionItem::ItemType::SEAUNITS_BEGIN && unitType < ProductionItem::ItemType::SEAUNITS_END) {
        return Object::Type::Sea;
    }
    return Object::Type::Land;
}


Object* BuildingEventProcessor::createUnitObject(ProductionItem::ItemType unitType) {
    // There is space, create and place unit
    TechnologyManager* pTechManager = m_pObjectOwner->getTechnologyCoordinator();
    Object* pObject = nullptr;

    switch (unitType) {
    case ProductionItem::ItemType::Peasant:
        pObject = new Peasant(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/units/peasant/ready.wav");
        else
            g_pSoundManager->playWAV("orc/peon/ready.wav");
        break;

    case ProductionItem::ItemType::Footman:
        pObject = new Footman(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/basic_voices/ready.wav");
        else
            g_pSoundManager->playWAV("orc/basic_voices/ready.wav");
        break;

    case ProductionItem::ItemType::Archer:
    {
        bool isRangerOrBeserker = pTechManager->isResearched(ProductionItem::ItemType::LumberMill_Ranger);
        pObject = new Archer(m_pObjectOwner, m_buildingFraction, m_pMap, isRangerOrBeserker);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/units/elven_archer-ranger/ready.wav");
        else
            g_pSoundManager->playWAV("orc/units/troll_axethrower-berserker/ready.wav");
    }
    break;

    case ProductionItem::ItemType::Church_Paladin:
        [[fallthrough]];
    case ProductionItem::ItemType::AltarOfStorms_OgreMage:
        [[fallthrough]];

    case ProductionItem::ItemType::Knight:
    {
        bool isPaladinOrOgreMage = pTechManager->isResearched(ProductionItem::ItemType::Church_Paladin);
        pObject = new Knight(m_pObjectOwner, m_buildingFraction, m_pMap, isPaladinOrOgreMage);

        std::string soundPath;
        if (m_buildingFraction == Fraction::Alliance) {
            soundPath = isPaladinOrOgreMage ? "human/units/paladin/ready.wav" : "human/units/knight/ready.wav";
        }
        else {
            soundPath = isPaladinOrOgreMage ? "orc/units/ogre-magre/ready.wav" : "orc/units/ogre/ready.wav";
        }

        g_pSoundManager->playWAV(soundPath);
    }
    break;

    case ProductionItem::ItemType::Ballista:
        pObject = new Ballista(m_pObjectOwner, m_buildingFraction, m_pMap);
        g_pSoundManager->playWAV("units/catapult-ballista/acknowledgement/1.wav"); // Catapult and Ballista share the same single sound
        break;

    case ProductionItem::ItemType::Mage:
        pObject = new Mage(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/units/mage/ready.wav");
        else
            g_pSoundManager->playWAV("orc/units/death_knight/ready.wav");
        break;

    case ProductionItem::ItemType::Dwarfes:
        pObject = new Dwarfes(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/units/dwarven_demolition_squad/ready.wav");
        else
            g_pSoundManager->playWAV("orc/units/goblin_sappers/ready.wav");
        break;

    case ProductionItem::ItemType::GnomishFlyingMaschine:
        pObject = new GnomishFlyingMaschine(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/units/gnomish_flying_machine/ready.wav");
        else
            g_pSoundManager->playWAV("orc/units/goblin_zeppelin/ready.wav");
        break;

    case ProductionItem::ItemType::GryphonRider:
        pObject = new GryphonRider(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/units/gryphon_rider/ready.wav");
        else
            g_pSoundManager->playWAV("orc/units/dragon/ready.wav");
        break;

    case ProductionItem::ItemType::Tanker:
        pObject = new Tanker(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/ships/ready.wav");
        else
            g_pSoundManager->playWAV("orc/ships/ready.wav");
        break;

    case ProductionItem::ItemType::Transport:
        pObject = new Transport(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/ships/ready.wav");
        else
            g_pSoundManager->playWAV("orc/ships/ready.wav");
        break;

    case ProductionItem::ItemType::Destroyer:
        pObject = new Destroyer(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/ships/ready.wav");
        else
            g_pSoundManager->playWAV("orc/ships/ready.wav");
        break;

    case ProductionItem::ItemType::Battleship:
        pObject = new Battleship(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/ships/ready.wav");
        else
            g_pSoundManager->playWAV("orc/ships/ready.wav");
        break;

    case ProductionItem::ItemType::GnomishSubmarine:
        pObject = new GnomishSubmarine(m_pObjectOwner, m_buildingFraction, m_pMap);
        if (m_buildingFraction == Fraction::Alliance)
            g_pSoundManager->playWAV("human/ships/ready.wav");
        else
            g_pSoundManager->playWAV("orc/ships/ready.wav");
        break;

    default:
        throw std::logic_error("Cannot create unit: Unknown unit type!");
        break;
    }

    return pObject;
}
