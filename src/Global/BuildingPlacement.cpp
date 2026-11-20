#include "BuildingPlacement.h"

#include "Buildings/Barracks.h"
#include "Buildings/Blacksmith.h"
#include "Buildings/Church.h"
#include "Buildings/Farm.h"
#include "Buildings/Foundry.h"
#include "Buildings/GnomishInventor.h"
#include "Buildings/GryphonAviary.h"
#include "Buildings/LumberMill.h"
#include "Buildings/OilRefinery.h"
#include "Buildings/ScoutTower.h"
#include "Buildings/Shipyard.h"
#include "Buildings/Stables.h"
#include "Buildings/Townhall.h"
#include "Global/GlobalAttributes.h"
#include <stdexcept>

bool isBuildingAction(PeasantActions action) {
    return action > PeasantActions::BuildBegin && action < PeasantActions::BuildEnd;
}

bool isCoastalBuildingAction(PeasantActions action) {
    return action == PeasantActions::BuildShipyard ||
           action == PeasantActions::BuildFoundry ||
           action == PeasantActions::BuildOilRefinery;
}

bool requiresOilPatchDistance(PeasantActions action) {
    return action == PeasantActions::BuildShipyard ||
           action == PeasantActions::BuildOilRefinery;
}

std::type_index buildingTypeForAction(PeasantActions action) {
    switch (action) {
    case PeasantActions::BuildTownhall: return typeid(Townhall);
    case PeasantActions::BuildFarm: return typeid(Farm);
    case PeasantActions::BuildBarracks: return typeid(Barracks);
    case PeasantActions::BuildLumberMill: return typeid(LumberMill);
    case PeasantActions::BuildBlacksmith: return typeid(Blacksmith);
    case PeasantActions::BuildChurch: return typeid(Church);
    case PeasantActions::BuildStables: return typeid(Stables);
    case PeasantActions::BuildGnomishInventor: return typeid(GnomishInventor);
    case PeasantActions::BuildGryphonAviary: return typeid(GryphonAviary);
    case PeasantActions::BuildScoutTower: return typeid(ScoutTower);
    case PeasantActions::BuildShipyard: return typeid(Shipyard);
    case PeasantActions::BuildFoundry: return typeid(Foundry);
    case PeasantActions::BuildOilRefinery: return typeid(OilRefinery);
    default: throw std::invalid_argument("not a building action");
    }
}

int buildingTileSizeForAction(PeasantActions action) {
    return gMapUnitTilesize.at(buildingTypeForAction(action));
}
