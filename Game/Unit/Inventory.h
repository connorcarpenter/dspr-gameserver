#pragma once

//
// Created by connor on 11/9/18.
//

#include "Unit.h"

namespace DsprGameServer {

    enum ItemSlot { Head, Body, LeftHand, RightHand, Belt1, Belt2};

    class Item;

    class Inventory {

    public:
        Inventory(DsprGameServer::Unit *masterUnit);
        ~Inventory();
        void addItem(Item *item);
        Item* getItem(ItemSlot slot);

        bool isDirty();
        std::string getUpdate(bool overrideDirty);
        void clean();

        Unit *masterUnit = nullptr;
        Item** items = nullptr;
        bool dirty = false;

        void swapSlots(int beforeIndex, int afterIndex);
    };
}