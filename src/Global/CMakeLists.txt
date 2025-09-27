# war25 Global component library
set(GLOBAL_SOURCES
    Action.cpp
    Action.h
    DebugManager.cpp
    DebugManager.h
    GlobalAttributes.cpp
    GlobalAttributes.h
    Object.cpp
    Object.h
    PseudoRandom.cpp
    PseudoRandom.h
    Resources.cpp
    Resources.h
    ProductionItem.cpp
    ProductionItem.h
    TechnologyManager.cpp
    TechnologyManager.h
    UpgradeEffect.cpp
    UpgradeEffect.h
    Viewport.cpp
    Viewport.h
    Upgrade.h
)

add_library(war25_global OBJECT ${GLOBAL_SOURCES})
target_link_libraries(war25_global PUBLIC war25_common)
