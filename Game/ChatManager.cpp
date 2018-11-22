//
// Created by connor on 11/15/18.
//

#include <sstream>
#include "ChatManager.h"
#include "../GameServer.h"
#include "../PlayerData.h"
#include "Tribe.h"
#include "../DsprMessage/ToClientMsg.h"

namespace DsprGameServer {

    ChatManager::ChatManager(Game *game) {
        this->game = game;
    }

    void ChatManager::receiveMessage(std::string &msgReceived, PlayerData *sendPlayerData) {
        for (auto playerData : this->game->playerDataSet)
        {
            if (playerData == sendPlayerData) continue;

            DsprMessage::ChatSendClientMsgV1 chatSendClientMsgV1;
            chatSendClientMsgV1.tribeIndex.set(sendPlayerData->getTribe()->index);
            chatSendClientMsgV1.chatMsg.loadFromString(msgReceived);
            auto clientMsg = chatSendClientMsgV1.getToClientMessage();
            auto packedMsg = clientMsg->Pack();
            GameServer::get().queueMessageTrue(playerData, packedMsg);
        }
    }
}