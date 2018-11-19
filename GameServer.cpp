//
// Created by connor on 7/25/18.
//

#include "GameServer.h"
#include "DsprMessage/ToClientMsg.h"

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
        char* newCstr = new char[str.length()];
        for(int i=0;i<str.length();i++)
            newCstr[i] = str[i];
        auto clientMessage = DsprMessage::ToClientMsg();
        clientMessage.msgType.set(DsprMessage::ToClientMsg::StandardMessage);
        DsprMessage::_cstr standardCstr(newCstr, str.length(), false);
        clientMessage.msgBytes.set(standardCstr);

        auto serializedClientMsg = clientMessage.Serialize();
        std::string msgStr = std::basic_string<char>(serializedClientMsg.innerCstr, serializedClientMsg.number);

        auto newMsg = new Message(playerData, std::move(msgStr));
        messageQueue.push(newMsg);
    }

    void GameServer::queueMessageTrue(PlayerData *playerData, std::string str) {
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