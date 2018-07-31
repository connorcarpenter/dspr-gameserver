#pragma once

//
// Created by connor on 7/25/18.
//

#include <uWS/WebSocketProtocol.h>
#include <uWS/WebSocket.h>

namespace DsprGameServer {
    class Message {
    public:
        Message(uWS::WebSocket<uWS::SERVER> *ws, std::string str);

        void send();

    private:
        uWS::WebSocket<uWS::SERVER> *ws;
        std::string str;
    };
}