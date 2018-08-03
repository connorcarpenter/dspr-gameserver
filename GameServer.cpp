//
// Created by connor on 7/25/18.
//

#include "GameServer.h"

namespace DsprGameServer
{
    GameServer::GameServer()
    {
        auto onlyGame = new Game();
        gameList.push_back(onlyGame);
        playerCodeToGameMap.insert(std::pair<std::string, Game*>("player1code", onlyGame));
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

    void GameServer::queueMessage(uWS::WebSocket<uWS::SERVER> *ws, std::string str)
    {
        auto newMsg = new Message(ws, std::move(str));
        messageQueue.push(newMsg);
    }
}