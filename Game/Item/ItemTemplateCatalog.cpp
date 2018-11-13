//
// Created by connor on 11/13/18.
//

#include "ItemTemplateCatalog.h"

namespace DsprGameServer {

    ItemTemplateCatalog::ItemTemplateCatalog(DsprGameServer::Game *game) {

        this->game = game;

        this->sling = new ItemTemplate(0);
        this->sling->wornOn = BodyPart::Hand;
        this->templateMap.emplace(this->sling->index, this->sling);

        this->club = new ItemTemplate(2);
        this->club->wornOn = BodyPart::Hand;
        this->templateMap.emplace(this->club->index, this->club);

        this->helmet = new ItemTemplate(3);
        this->helmet->wornOn = BodyPart::Head;
        this->templateMap.emplace(this->helmet->index, this->helmet);

        this->shield = new ItemTemplate(4);
        this->shield->wornOn = BodyPart::Hand;
        this->templateMap.emplace(this->shield->index, this->shield);

        this->armor = new ItemTemplate(5);
        this->armor->wornOn = BodyPart::Body;
        this->templateMap.emplace(this->armor->index, this->armor);
    }

    ItemTemplateCatalog::~ItemTemplateCatalog() {
        for(const auto& itemPair : this->templateMap)
        {
            ItemTemplate* itemTemplate = itemPair.second;
            delete itemTemplate;
        }
    }

    ItemTemplate *ItemTemplateCatalog::getTemplateFromIndex(int index) {
        if (this->templateMap.count(index) <= 0) return nullptr;
        return this->templateMap.at(index);
    }
}