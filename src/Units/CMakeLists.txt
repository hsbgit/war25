# war25 Units component library
add_subdirectory(AirUnits)
add_subdirectory(LandUnits)
add_subdirectory(SeaUnits)
add_subdirectory(Resource)

set(UNITS_SOURCES
    AttackerStats.h
    Critter.cpp
    Critter.h
    IArmed.cpp
    IArmed.h
    Projectile.cpp
    Projectile.h
    Unit.cpp
    Unit.h
    UnitActions.h
)

add_library(war25_units OBJECT ${UNITS_SOURCES})
target_link_libraries(war25_units PUBLIC war25_common)

