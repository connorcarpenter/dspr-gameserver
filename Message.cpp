//
// Created by connor on 7/25/18.
//

#include <iostream>
#include "Message.h"
#include "PlayerData.h"

namespace DsprGameServer {
    Message::Message(PlayerData *playerData, DsprMessage::_cstr str) : str(str) {
        this->playerData = playerData;
    }

    void Message::send() {
        this->playerData->getWs()->send((char*) str.innerCstr, str.number, uWS::BINARY);

        //TODO:: REMOVE FOR PRODUCTION!
        std::string strr = std::basic_string<char>((char*) str.innerCstr, str.number);
        std::cout << "gameserver: sent: " << strr << std::endl;
    }
}