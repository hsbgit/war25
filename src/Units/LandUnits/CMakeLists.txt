# war25 LandUnits component library
add_subdirectory(Dwarfes)
add_subdirectory(Peasant)

set(LAND_UNITS_SOURCES
    Archer.cpp
    Archer.h
    Ballista.cpp
    Ballista.h
    Footman.cpp
    Footman.h
    Knight.cpp
    Knight.h
    Mage.cpp
    Mage.h
    Skeleton.cpp
    Skeleton.h
)

add_library(war25_units_land OBJECT ${LAND_UNITS_SOURCES})
target_link_libraries(war25_units_land PUBLIC war25_common)


