# war25 AirUnits component library
set(AIR_UNITS_SOURCES
    GnomishFlyingMachine.cpp
    GnomishFlyingMachine.h
    GryphonRider.cpp
    GryphonRider.h
)

add_library(war25_units_air OBJECT ${AIR_UNITS_SOURCES})
target_link_libraries(war25_units_air PUBLIC war25_common)

