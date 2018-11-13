//
// Created by connor on 11/8/18.
//

#include "Item.h"
#include "ItemManager.h"
#include "../FogManager.h"
#include "ItemTemplate.h"

namespace DsprGameServer {

    Item::Item(DsprGameServer::Game *game, int id, int x, int y, ItemTemplate *itemTemplate) {
        this->game = game;
        this->position = new Point(x,y);
        this->itemTemplate = itemTemplate;
        this->id = id;

        this->game->itemManager->addItemToGrid(this);
    }

    Item::~Item(){
        delete this->position;
    }

    bool Item::isVisibleToTribe(Tribe *tribe) {
        return this->game->fogManager->tileIsClear(tribe, this->position->x, this->position->y);
    }
}