#pragma once

//
// Created by connor on 7/23/18.
//

#include <uWS/WebSocket.h>
#include <map>
#include "Player.h"
#include "TileManager.h"
#include "UnitManager.h"

namespace DsprGameServer {
    class Game {

    public:
        Game();

        ~Game();

        void addPlayer(std::string token, uWS::WebSocket<1> *ws);
        void receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY);
        void update();

    private:
        std::map<std::string, Player *> playerMap;
        TileManager* tileManager = nullptr;
        UnitManager* unitManager = nullptr;
    };
}