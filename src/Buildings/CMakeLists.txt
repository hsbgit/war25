# war25 Buildings component library
set(BUILDINGS_SOURCES
    Barracks.cpp
    Barracks.h
    Blacksmith.cpp
    Blacksmith.h
    Building.cpp
    Building.h
    Church.cpp
    Church.h
    DarkPortal.cpp
    DarkPortal.h
    Farm.cpp
    Farm.h
    GnomishInventor.cpp
    GnomishInventor.h
    GryphonAviary.cpp
    GryphonAviary.h
    Foundry.cpp
    Foundry.h
    Goldmine.cpp
    Goldmine.h
    LumberMill.cpp
    LumberMill.h
    MageTower.cpp
    MageTower.h
    OilRefinery.cpp
    OilRefinery.h
    OilRig.cpp
    OilRig.h
    ScoutTower.cpp
    ScoutTower.h
    Shipyard.cpp
    Shipyard.h
    Stables.cpp
    Stables.h
    Townhall.cpp
    Townhall.h
)

add_library(war25_buildings OBJECT ${BUILDINGS_SOURCES})
target_link_libraries(war25_buildings PUBLIC war25_common)
