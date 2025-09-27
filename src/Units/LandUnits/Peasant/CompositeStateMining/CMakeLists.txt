# war25 Peasant Mining component library
set(MINING_SOURCES
    CompositeStateMining.cpp
    CompositeStateMining.h
    StateCollectGold.cpp
    StateCollectGold.h
    StateEnterBuildingForDelivery.cpp
    StateEnterBuildingForDelivery.h
    StateMoveToGoldmine.cpp
    StateMoveToGoldmine.h
    StateReturningResource.cpp
    StateReturningResource.h
)

add_library(war25_peasant_mining OBJECT ${MINING_SOURCES})
target_link_libraries(war25_peasant_mining PUBLIC war25_common)


