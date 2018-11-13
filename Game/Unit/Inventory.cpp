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
        delete [] this->items;
    }

    void Inventory::addItem(Item *item) {
        for(int i=0;i<6;i++)
        {
            if(this->items[i] == nullptr)
            {
                this->items[i] = item;
                this->dirty = true;
                return;
            }
        }
    }

    void Inventory::removeItem(Item *item) {
        for(int i=0;i<6;i++)
        {
            if(this->items[i] == item)
            {
                this->items[i] = nullptr;
                this->dirty = true;
                return;
            }
        }
    }

    Item* Inventory::getItem(ItemSlot slot){
        return this->items[slot];
    }

    Item* Inventory::getItem(int slotIndex){
        return this->items[slotIndex];
    }

    bool Inventory::isDirty() {
        return dirty;
    }

    void Inventory::clean() {
        this->dirty = false;
    }

    std::string Inventory::getUpdate(bool overrideDirty) {
        bool first = true;
        std::string newStr;
        newStr.append("inventory:");
        for(int i = 0;i<6;i++)
        {
            if (!first) newStr.append(",");

            if (this->items[i] == nullptr)
            {
                newStr.append("-1");
            }
            else
            {
                newStr.append(std::to_string(this->items[i]->index));
            }

            first = false;
        }
        return newStr;
    }

    void Inventory::swapSlots(int beforeIndex, int afterIndex) {
        auto itemA = this->items[beforeIndex];
        auto itemB = this->items[afterIndex];

        this->items[beforeIndex] = itemB;
        this->items[afterIndex] = itemA;

        this->dirty = true;
    }

    void Inventory::removeItemAtSlot(int slotIndex) {
        this->items[slotIndex] = nullptr;
    }
}