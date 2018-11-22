#pragma once

//
// Created by connor on 7/25/18.
//

#include <uWS/WebSocketProtocol.h>
#include <uWS/WebSocket.h>
#include "DsprMessage/Number.h"

namespace DsprGameServer {

    class PlayerData;

    class Message {
    public:
        Message(PlayerData *playerData, std::shared_ptr<DsprMessage::CStr> str);

        void send();

    private:
        PlayerData* playerData = nullptr;
        std::shared_ptr<DsprMessage::CStr> str = nullptr;
    };
}