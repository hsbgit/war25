# war25 Peasant ConstructBuilding component library
set(CONSTRUCTBUILDING_SOURCES
    CompositeStateConstructBuilding.cpp
    CompositeStateConstructBuilding.h
)

add_library(war25_peasant_constructbuilding OBJECT ${CONSTRUCTBUILDING_SOURCES})
target_link_libraries(war25_peasant_constructbuilding PUBLIC war25_common)


