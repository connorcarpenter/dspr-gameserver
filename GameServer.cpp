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
        unsigned char* newCstr = new unsigned char[str.length()];
        for(int i=0;i<str.length();i++)
            newCstr[i] = str[i];
        auto clientMessage = DsprMessage::ToClientMsg();

        assert(DsprMessage::ToClientMsg::MessageType::MessageTypeMaxValue < DsprMessage::MaxByteValue);
        clientMessage.msgType.set((unsigned char) DsprMessage::ToClientMsg::MessageType::StandardMessage);

        DsprMessage::_cstr standardCstr(newCstr, str.length(), false);
        clientMessage.msgBytes.setCstr(standardCstr);

        auto serializedClientMsg = clientMessage.Serialize();

        auto newMsg = new Message(playerData, serializedClientMsg);
        messageQueue.push(newMsg);
    }

    void GameServer::queueMessageTrue(PlayerData *playerData, DsprMessage::_cstr str) {
        auto newMsg = new Message(playerData, str);
        messageQueue.push(newMsg);
    }

    void GameServer::addPlayerToken(std::string playerToken) {
        playerCodeToGameMap.insert(std::pair<std::string, Game*>(playerToken, this->onlyGame));
    }

    void GameServer::removePlayerToken(std::string playerToken) {
        playerCodeToGameMap.erase(playerToken);
    }


}