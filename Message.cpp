//
// Created by connor on 7/25/18.
//

#include <iostream>
#include "Message.h"

Message::Message(uWS::WebSocket<uWS::SERVER> *ws, std::string str) {
    this->ws = ws;
    this->str = str;
}

void Message::send() {
    this->ws->send(str.c_str(), uWS::BINARY);
    std::cout << "gameserver: sent: " << str << std::endl;
}


