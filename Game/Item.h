#pragma once

//
// Created by connor on 11/8/18.
//

#include "Game.h"
#include "../Math/Point.h"

namespace DsprGameServer {
    class Item {

    public:
        Item(DsprGameServer::Game *game, int id, int x, int y, int index);
        ~Item();

        Game *game = nullptr;
        Point* position = nullptr;
        int index;
        int id;

        bool isVisibleToTribe(Tribe *tribe);
    };
}