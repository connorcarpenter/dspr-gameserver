//
// Created by connor on 7/25/18.
//

#include <iostream>
#include "Message.h"
#include "PlayerData.h"

namespace DsprGameServer {
    Message::Message(PlayerData *playerData, std::string str) {
        this->playerData = playerData;
        this->str = str;
    }

    void Message::send() {
        this->playerData->getWs()->send(str.c_str(), uWS::BINARY);
        std::cout << "gameserver: sent: " << str << std::endl;
    }
}