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
        Game.h Game.cpp
        Player.h Player.cpp
        TileManager.h TileManager.cpp
        UnitManager.h UnitManager.cpp
        Tile.h Tile.cpp
        Unit.h Unit.cpp
        Point.h Point.cpp
        Synced.h
        StringUtils.h MapUtils.h MathUtils.h)

target_link_libraries (dspr-gameserver LINK_PUBLIC libuWS libssl libz)