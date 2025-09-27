# war25 Peasant ChopWood component library
set(CHOPWOOD_SOURCES
    CompositeStateChopWood.cpp
    CompositeStateChopWood.h
    StateChopWood.cpp
    StateChopWood.h
    StateMoveToWood.cpp
    StateMoveToWood.h
)

add_library(war25_peasant_chopwood OBJECT ${CHOPWOOD_SOURCES})
target_link_libraries(war25_peasant_chopwood PUBLIC war25_common)


