#pragma once

//
// Created by connor on 11/13/18.
//

#include <map>
#include "ItemTemplate.h"
#include "../Game.h"

namespace DsprGameServer {
    class ItemTemplateCatalog {
    public:

        ItemTemplateCatalog(DsprGameServer::Game *game);

        ~ItemTemplateCatalog();
        ItemTemplate* getTemplateFromIndex(int index);

        ItemTemplate *sling = nullptr;
        ItemTemplate *club = nullptr;
        ItemTemplate *helmet = nullptr;
        ItemTemplate *shield = nullptr;
        ItemTemplate *armor = nullptr;

    private:
        std::map<int, ItemTemplate*> templateMap;

        Game *game;
    };
}