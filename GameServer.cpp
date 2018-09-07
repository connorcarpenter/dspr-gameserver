//
// Created by connor on 7/25/18.
//

#include "GameServer.h"

namespace DsprGameServer
{
    GameServer::GameServer()
    {
        this->onlyGame = new Game();
        gameList.push_back(onlyGame);

    }

    void GameServer::step()
    {
        // update games
        for(const auto& game : gameList)
        {
            game->update();
        }

        // send messages
        for (int i = 0; i < 1000; i += 1) {
            if (messageQueue.size() == 0)
                break;
            Message *msg = messageQueue.front();
            msg->send();
            messageQueue.pop();
            delete msg;
        }
    }

    void GameServer::queueMessage(PlayerData *playerData, std::string str)
    {
        auto newMsg = new Message(playerData, std::move(str));
        messageQueue.push(newMsg);
    }

    void GameServer::addPlayerToken(std::string playerToken) {
        playerCodeToGameMap.insert(std::pair<std::string, Game*>(playerToken, this->onlyGame));
    }

    void GameServer::removePlayerToken(std::string playerToken) {
        playerCodeToGameMap.erase(playerToken);
    }
}