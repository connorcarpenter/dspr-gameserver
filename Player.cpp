//
// Created by connor on 7/23/18.
//

#include "Player.h"

namespace DsprGameServer {
    Player::Player(std::string token, uWS::WebSocket<1> *ws) {
        this->ws = ws;
        this->token = token;
    }

    uWS::WebSocket<1> * Player::getWs()
    {
        return this->ws;
    }
}