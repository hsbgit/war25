#pragma once

#include "Units/UnitActions.h"
#include <typeindex>

bool isBuildingAction(PeasantActions action);
bool isCoastalBuildingAction(PeasantActions action);
bool requiresOilPatchDistance(PeasantActions action);
std::type_index buildingTypeForAction(PeasantActions action);
int buildingTileSizeForAction(PeasantActions action);
