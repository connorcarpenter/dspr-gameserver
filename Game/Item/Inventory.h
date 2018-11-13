#pragma once

//
// Created by connor on 11/9/18.
//

#include "../Unit/Unit.h"
#include "BodyPart.h"

namespace DsprGameServer {

    class Item;

    class Inventory {

    public:
        Inventory(DsprGameServer::Unit *masterUnit);
        ~Inventory();
        void addItem(Item *item);
        Item* getItem(int slot);
        void swapSlots(int beforeIndex, int afterIndex);
        void removeItem(Item *item);
        bool canPlaceInSlot(int slotIndex, Item *item);

        bool isDirty();
        std::string getUpdate(bool overrideDirty);
        void clean();

        Unit *masterUnit = nullptr;
        Item** items = nullptr;
        bool dirty = false;

        Item* itemToDrop = nullptr;

    private:
        static const BodyPart slotAllowsPart(int index){
            static const BodyPart slotAllowsArr[] = { Head, Hand, Hand, Body, Any, Any };
            return slotAllowsArr[index];
        }

        void setItemInSlot(int slotIndex, Item *item);
    };
}