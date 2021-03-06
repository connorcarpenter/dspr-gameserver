//
// Created by connor on 11/9/18.
//

#include "Inventory.h"
#include "Item.h"

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
            if(this->items[i] == nullptr && this->canPlaceInSlot(i, item))
            {
                setItemInSlot(i, item);
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

    Item* Inventory::getItem(int slot){
        return this->items[slot];
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
                newStr.append(std::to_string(this->items[i]->itemTemplate->index));
            }

            first = false;
        }
        return newStr;
    }

    void Inventory::setItemInSlot(int slotIndex, Item* item){
        if (!canPlaceInSlot(slotIndex, item)) {
            int i = 1/0; //how did we get here?
        }
        this->items[slotIndex] = item;
        this->dirty = true;
    }

    void Inventory::swapSlots(int beforeIndex, int afterIndex) {
        auto itemA = this->items[beforeIndex];
        auto itemB = this->items[afterIndex];

        if (canPlaceInSlot(beforeIndex, itemB) && canPlaceInSlot(afterIndex, itemA)) {
            setItemInSlot(beforeIndex, itemB);
            setItemInSlot(afterIndex, itemA);
        }

        this->dirty = true;
    }

    bool Inventory::canPlaceInSlot(int slotIndex, Item *item) {
        auto partAllowed = this->slotAllowsPart(slotIndex);
        if (partAllowed == Any)return true;
        if (partAllowed == None)return false;
        if (item == nullptr) return true;
        if (partAllowed == item->itemTemplate->wornOn)return true;
        return false;
    }
}