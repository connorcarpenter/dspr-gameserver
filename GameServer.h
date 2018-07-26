#pragma once

//
// Created by connor on 7/25/18.
//

#include <queue>
#include "Message.h"

class GameServer {

public:
    void step();
    void queueMessage(uWS::WebSocket<uWS::SERVER> *ws, std::string str);
    static GameServer& get()
    {
        static GameServer gameServer;
        return gameServer;
    }
private:
    GameServer(){};
    std::queue<Message*> messageQueue;
};
