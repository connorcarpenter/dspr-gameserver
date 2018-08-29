cmake_minimum_required(VERSION 3.9)
project(dspr-gameserver)

add_library(libuWS SHARED IMPORTED)
set_property(TARGET libuWS PROPERTY IMPORTED_LOCATION "/usr/lib/libuWS.so")

add_library(libssl SHARED IMPORTED)
set_property(TARGET libssl PROPERTY IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libssl.so")

add_library(libz SHARED IMPORTED)
set_property(TARGET libz PROPERTY IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libz.so")

add_library(ssl INTERFACE)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(dspr-gameserver
        main.cpp
        GameServer.h GameServer.cpp
        Message.h Message.cpp
        Player.h Player.cpp
        StringUtils.h
        MapUtils.h

        Game/Game.h Game/Game.cpp
        Game/TileManager.h Game/TileManager.cpp
        Game/UnitManager.h Game/UnitManager.cpp
        Game/Tile.h Game/Tile.cpp
        Game/Unit.h Game/Unit.cpp
        Game/OrderGroup.h Game/OrderGroup.cpp
        Game/Synced.h
        Game/Tribe.h Game/Tribe.cpp
        Game/TribeManager.h Game/TribeManager.cpp

        Math/Point.h Math/Point.cpp
        Math/FloatPoint.h Math/FloatPoint.cpp
        Math/MathUtils.h

        Pathfinding/AStarPathfinder.h Pathfinding/AStarPathfinder.cpp
        Pathfinding/Path.h Pathfinding/Path.cpp
        Pathfinding/PathNode.h Pathfinding/PathNode.cpp
        Pathfinding/PathTile.h Pathfinding/PathTile.cpp)

target_link_libraries (dspr-gameserver LINK_PUBLIC libuWS libssl libz)