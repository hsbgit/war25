# war25 Resource component library
set(RESOURCE_SOURCES
    IResourceAcceptor.cpp
    IResourceAcceptor.h
    IResourceMine.cpp
    IResourceMine.h
    ResourceType.h
)

add_library(war25_units_resource OBJECT ${RESOURCE_SOURCES})
target_link_libraries(war25_units_resource PUBLIC war25_common)

