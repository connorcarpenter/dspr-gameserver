#pragma once

//
// Created by connor on 7/25/18.
//

#include <queue>
#include "Message.h"
#include "Game.h"

namespace DsprGameServer {

    class GameServer {

    public:
        void step();

        void queueMessage(uWS::WebSocket<uWS::SERVER> *ws, std::string str);

        static GameServer& get() {
            static GameServer gameServer;
            return gameServer;
        }

        std::map<std::string, Game*> playerCodeToGameMap;

        int tickMs = 200;
    private:
        GameServer();
        std::queue<Message *> messageQueue;
        std::list<Game*> gameList;

    };
}