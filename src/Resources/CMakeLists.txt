# war25 Resources component library
set(RESOURCES_SOURCES
    Animation.cpp
    Animation.h
    resourcemanager_wartools.cpp
    resourcemanager_wartools.h
    ResourceMappings.cpp
    ResourceMappings.h
    Sound.cpp
    Sound.h
    SoundCache.cpp
    SoundCache.h
    SoundManager.cpp
    SoundManager.h
)

add_library(war25_resources OBJECT ${RESOURCES_SOURCES})
target_link_libraries(war25_resources PUBLIC war25_common)
