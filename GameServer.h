#pragma once

//
// Created by connor on 7/25/18.
//

#include <queue>
#include "Message.h"
#include "Game.h"

namespace DsprGameServer
{
    class GameServer
    {

    public:

        static GameServer& get() {
            static GameServer gameServer;
            return gameServer;
        }

        void step();
        void queueMessage(uWS::WebSocket<uWS::SERVER> *ws, std::string str);
        void addPlayer(std::string playerToken);

        std::map<std::string, Game*> playerCodeToGameMap;

        int tickMs = 100;

    private:

        GameServer();
        std::queue<Message *> messageQueue;
        std::list<Game*> gameList;

        Game *onlyGame;
    };
}