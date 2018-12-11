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
        PlayerData.h PlayerData.cpp
        StringUtils.h
        MapUtils.h
        PtrIsoGrid.h PrimIsoGrid.h
        MessageReceiver.h MessageSender.h

        Game/Game.h Game/Game.cpp
        Game/TileManager.h Game/TileManager.cpp
        Game/Tile.h Game/Tile.cpp
        Game/Tribe.h Game/Tribe.cpp
        Game/TribeManager.h Game/TribeManager.cpp
        Game/FogManager.h Game/FogManager.cpp
        Game/EconomyManager.h Game/EconomyManager.cpp
        Game/ChatManager.h Game/ChatManager.cpp
        Game/VisionProfile.h Game/VisionProfile.cpp

        Game/Item/Item.h Game/Item/Item.cpp
        Game/Item/ItemManager.h Game/Item/ItemManager.cpp
        Game/Item/ItemTemplate.h Game/Item/ItemTemplate.cpp
        Game/Item/ItemTemplateCatalog.h Game/Item/ItemTemplateCatalog.cpp
        Game/Item/Inventory.h Game/Item/Inventory.cpp
        Game/Item/BodyPart.h

        Game/Unit/UnitManager.h Game/Unit/UnitManager.cpp
        Game/Unit/Unit.h Game/Unit/Unit.cpp
        Game/Unit/OrderGroup.h Game/Unit/OrderGroup.cpp
        Game/Unit/Synced.h
        Game/Unit/AnimationState.h Game/Unit/AnimationState.cpp
        Game/Unit/UnitOrder.h Game/Unit/UnitOrder.cpp
        Game/Unit/UnitTemplateCatalog.h Game/Unit/UnitTemplateCatalog.cpp
        Game/Unit/UnitTemplate.h Game/Unit/UnitTemplate.cpp
        Game/Unit/ConstructionQueue.h Game/Unit/ConstructionQueue.cpp
        Game/Unit/Projectile.h Game/Unit/Projectile.cpp

        Game/Unit/SpecificUnit/SpecificUnit.h Game/Unit/SpecificUnit/SpecificUnit.cpp
        Game/Unit/SpecificUnit/Manafount.h Game/Unit/SpecificUnit/Manafount.cpp
        Game/Unit/SpecificUnit/TempleBuilding.h Game/Unit/SpecificUnit/TempleBuilding.cpp
        Game/Unit/SpecificUnit/TempleFlying.h Game/Unit/SpecificUnit/TempleFlying.cpp

        Game/Circle/CircleCache.h Game/Circle/CircleCache.cpp
        Game/Circle/Circle.h Game/Circle/Circle.cpp
        Game/Circle/CircleCoord.h Game/Circle/CircleCoord.cpp

        Game/IsoBox/IsoBoxCache.h Game/IsoBox/IsoBoxCache.cpp
        Game/IsoBox/IsoBox.h Game/IsoBox/IsoBox.cpp

        Game/PlaneGeneration/PlaneGenerator.h Game/PlaneGeneration/PlaneGenerator.cpp

        DsprMessage/Main.h
        DsprMessage/ToServerMsg.h DsprMessage/ToServerMsg.cpp
        DsprMessage/ToClientMsg.h DsprMessage/ToClientMsg.cpp
        DsprMessage/Number.h DsprMessage/Number.cpp
        DsprMessage/Array.h DsprMessage/Array.cpp
        DsprMessage/CStr.h DsprMessage/CStr.cpp
        DsprMessage/CharVector.h DsprMessage/CharVector.cpp

        Math/Point.h Math/Point.cpp
        Math/FloatPoint.h Math/FloatPoint.cpp
        Math/MathUtils.h
        Math/Int.h Math/Int.cpp
        Math/Bool.h Math/Bool.cpp

        Pathfinding/AStarPathfinder.h Pathfinding/AStarPathfinder.cpp
        Pathfinding/SimplePathfinder.h Pathfinding/SimplePathfinder.cpp
        Pathfinding/Path.h Pathfinding/Path.cpp
        Pathfinding/PathNode.h Pathfinding/PathNode.cpp
        Pathfinding/PathTile.h Pathfinding/PathTile.cpp
        Pathfinding/MoveEndPosFiller.h Pathfinding/MoveEndPosFiller.cpp)

target_link_libraries (dspr-gameserver LINK_PUBLIC libuWS libssl libz)