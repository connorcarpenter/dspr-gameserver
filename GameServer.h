#pragma once

//
// Created by connor on 7/25/18.
//

#include <queue>
#include <map>
#include "Message.h"
#include "Game/Game.h"

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
        void queueMessage(PlayerData *playerData, std::string str);
        void queueMessageTrue(PlayerData *playerData, std::string str);
        void addPlayerToken(std::string playerToken);
        void removePlayerToken(std::string playerToken);

        std::map<std::string, Game*> playerCodeToGameMap;

        int tickMs = 100;

    private:

        GameServer();
        std::queue<Message *> messageQueue;
        std::list<Game*> gameList;

        Game *onlyGame;
    };
}