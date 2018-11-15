#pragma once

//
// Created by connor on 11/15/18.
//

#include "Game.h"

namespace DsprGameServer {
    class ChatManager {
    public:
        ChatManager(Game* game);

        Game *game = nullptr;

        void receiveMessage(std::string &basic_string, PlayerData *playerData);
    };
}