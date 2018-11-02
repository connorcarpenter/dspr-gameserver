#pragma once

//
// Created by connor on 7/23/18.
//

#include <uWS/WebSocket.h>
#include <list>
#include <set>


namespace DsprGameServer {

    class TileManager;
    class EconomyManager;
    class UnitManager;
    class TribeManager;
    class AStarPathfinder;
    class SimplePathfinder;
    class PlayerData;
    class FogManager;
    class Tribe;
    class UnitTemplateCatalog;

    class Game
    {

    public:

        Game();

        ~Game();

        void addPlayer(const std::string& token, PlayerData* playerData);
        void removePlayer(PlayerData* playerData);
        void update();

        TileManager* tileManager = nullptr;
        UnitManager* unitManager = nullptr;
        EconomyManager* economyManager = nullptr;
        TribeManager* tribeManager = nullptr;
        AStarPathfinder* pathfinder = nullptr;
        SimplePathfinder* simplePathfinder = nullptr;
        FogManager* fogManager = nullptr;
        UnitTemplateCatalog* unitTemplateCatalog = nullptr;

    private:

        std::set<PlayerData *> playerDataSet;

        void sendPlayerTribeIndex(PlayerData *playerData, DsprGameServer::Tribe *tribe);
    };
}