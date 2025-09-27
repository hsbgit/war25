# war25 Map component library
set(MAP_SOURCES
    AStar.cpp
    AStar.h
    INode.h
    Map.cpp
    Map.h
    MapInfo.cpp
    MapInfo.h
    MapObject.h
    Path.cpp
    Path.h
    Tile.cpp
    Tile.h
)

add_library(war25_map OBJECT ${MAP_SOURCES})
target_link_libraries(war25_map PUBLIC war25_common)

