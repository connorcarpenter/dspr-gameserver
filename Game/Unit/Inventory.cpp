//
// Created by connor on 11/9/18.
//

#include "Inventory.h"
#include "../Item/Item.h"

namespace DsprGameServer
{

    Inventory::Inventory(DsprGameServer::Unit *masterUnit) {
        this->masterUnit = masterUnit;
        this->items = new Item*[6];
        for(int i=0;i<6;i++)
            this->items[i] = nullptr;
    }

    Inventory::~Inventory() {
        delete this->items;
    }

    void Inventory::addItem(Item *item) {

    }

    Item* Inventory::getItem(ItemSlot slot){
        return this->items[slot];
    }
}