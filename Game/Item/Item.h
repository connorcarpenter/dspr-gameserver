#pragma once

//
// Created by connor on 11/8/18.
//

#include "../Game.h"
#include "../../Math/Point.h"
#include "ItemTemplate.h"

namespace DsprGameServer {
    class Item {

    public:
        Item(DsprGameServer::Game *game, int id, int x, int y, ItemTemplate *itemTemplate);
        ~Item();

        Game *game = nullptr;
        Point* position = nullptr;
        ItemTemplate *itemTemplate = nullptr;
        int id = -1;

        bool isVisibleToTribe(Tribe *tribe);
    };
}