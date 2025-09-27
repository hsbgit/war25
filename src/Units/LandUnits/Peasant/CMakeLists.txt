# war25 Peasant component library
add_subdirectory(CompositeStateChopWood)
add_subdirectory(CompositeStateConstructBuilding)
add_subdirectory(CompositeStateMining)

set(PEASANT_SOURCES
    Peasant.cpp
    Peasant.h
)

add_library(war25_units_peasant OBJECT ${PEASANT_SOURCES})
target_link_libraries(war25_units_peasant PUBLIC war25_common)


