# war25 Dwarfes component library
set(DWARFES_SOURCES
    Dwarfes.cpp
    Dwarfes.h
    StateExplode.cpp
    StateExplode.h
)

add_library(war25_units_dwarfes OBJECT ${DWARFES_SOURCES})
target_link_libraries(war25_units_dwarfes PUBLIC war25_common)


