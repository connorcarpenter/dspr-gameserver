//
// Created by connor on 11/15/18.
//

#include <sstream>
#include "ChatManager.h"
#include "../GameServer.h"

namespace DsprGameServer {

    ChatManager::ChatManager(Game *game) {
        this->game = game;
    }

    void ChatManager::receiveMessage(std::string &msgReceived, PlayerData *sendPlayerData) {
        for (auto playerData : this->game->playerDataSet)
        {
            if (playerData == sendPlayerData) continue;

            std::stringstream msg;
            msg << "chat/1.0/send|" << msgReceived << "\r\n";

            GameServer::get().queueMessage(playerData, msg.str());
        }
    }
}