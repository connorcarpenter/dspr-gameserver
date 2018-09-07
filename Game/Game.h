#pragma once

//
// Created by connor on 7/23/18.
//

#include <uWS/WebSocket.h>
#include <list>
#include <set>

namespace DsprGameServer {

    class TileManager;
    class UnitManager;
    class TribeManager;
    class AStarPathfinder;
    class SimplePathfinder;
    class PlayerData;

    class Game {

    public:
        Game();

        ~Game();

        void addPlayer(const std::string& token, PlayerData* playerData);
        void removePlayer(PlayerData* playerData);
        void update();

        TileManager* tileManager = nullptr;
        UnitManager* unitManager = nullptr;
        TribeManager* tribeManager = nullptr;
        AStarPathfinder* pathfinder = nullptr;
        SimplePathfinder *simplePathfinder;
    private:
        std::set<PlayerData *> playerDataSet;
    };
}