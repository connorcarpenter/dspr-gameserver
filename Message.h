#pragma once

//
// Created by connor on 7/25/18.
//

#include <uWS/WebSocketProtocol.h>
#include <uWS/WebSocket.h>
#include "DsprMessage/DataTypes.h"

namespace DsprGameServer {

    class PlayerData;

    class Message {
    public:
        Message(PlayerData *playerData, DsprMessage::_cstr str);

        void send();

    private:
        PlayerData* playerData = nullptr;
        DsprMessage::_cstr str;
    };
}