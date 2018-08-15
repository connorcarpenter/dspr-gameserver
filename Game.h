#pragma once

//
// Created by connor on 7/23/18.
//

#include <uWS/WebSocket.h>
#include <map>
#include <list>
#include "Player.h"

namespace DsprGameServer {

    class TileManager;
    class UnitManager;

    class Game {

    public:
        Game();

        ~Game();

        void addPlayer(std::string token, uWS::WebSocket<1> *ws);
        void receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY);
        void update();

        TileManager* tileManager = nullptr;
        UnitManager* unitManager = nullptr;
    private:
        std::map<std::string, Player *> playerMap;
    };
}