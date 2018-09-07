#pragma once

//
// Created by connor on 7/25/18.
//

#include <uWS/WebSocketProtocol.h>
#include <uWS/WebSocket.h>

namespace DsprGameServer {

    class PlayerData;

    class Message {
    public:
        Message(PlayerData *playerData, std::string str);

        void send();

    private:
        PlayerData* playerData = nullptr;
        std::string str;
    };
}