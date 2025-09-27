# war25 SeaUnits component library
set(SEA_UNITS_SOURCES
    Battleship.cpp
    Battleship.h
    Destroyer.cpp
    Destroyer.h
    GnomishSubmarine.cpp
    GnomishSubmarine.h
    Tanker.cpp
    Tanker.h
    Transport.cpp
    Transport.h
)

add_library(war25_units_sea OBJECT ${SEA_UNITS_SOURCES})
target_link_libraries(war25_units_sea PUBLIC war25_common)

