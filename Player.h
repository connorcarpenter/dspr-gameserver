#pragma once

//
// Created by connor on 7/23/18.
//

#include <uWS/WebSocket.h>

class Player {
public:
    explicit Player(std::string token, uWS::WebSocket<1>* ws);
    uWS::WebSocket<1>* getWs();
private:
    uWS::WebSocket<1>* ws = nullptr;
    std::string token;
};