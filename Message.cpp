//
// Created by connor on 7/25/18.
//

#include <iostream>
#include "Message.h"
#include "PlayerData.h"

namespace DsprGameServer {
    Message::Message(PlayerData *playerData, std::shared_ptr<DsprMessage::CStr> str) : str(str) {
        this->playerData = playerData;
    }

    void Message::send() {
        this->playerData->getWs()->send((char*) str->getCharPtr(), str->size(), uWS::BINARY);

        //TODO:: REMOVE FOR PRODUCTION!
        //std::string strr = std::basic_string<char>((char*) str->getCharPtr(), str->size());
        //std::cout << ">" << std::endl;
    }
}