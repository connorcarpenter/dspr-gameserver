#pragma once
//
// Created by connor on 11/8/18.
//

#include "../../PtrIsoGrid.h"
#include "Item.h"
#include <map>

namespace DsprGameServer {
    class Game;

    class ItemManager {
    public:
        ItemManager(Game* game);
        ~ItemManager();

        void initializeFirstItems();

        void addItemToGrid(Item *item);

        void addPlayer(PlayerData *playerData);
        void removePlayer(PlayerData *playerData);

        void initSendAllItems(PlayerData *player);

        void sendItemUpdates(PlayerData *playerData);
        bool itemExists(int itemId);

        Item *getItem(int id);

        void removeItem(Item *item);

        void createItem(int x, int y, int templateIndex);

        bool canPlaceItemAtPos(int x, int y);

        void dropItemFromInventory(int x, int y, Item *item);

    private:
        Game *game = nullptr;

        PtrIsoGrid<Item*>* itemGrid = nullptr;

        std::map<PlayerData*, std::set<Item*>*> playerToItemsAwareOfMap;

        std::map<int, Item*> itemMap;

        void setItemInGrid(int x, int y, Item *item);

        void makePlayerAwareOfItem(PlayerData *playerData, Item *item);

        void makePlayerUnawareOfItem(PlayerData *playerData, Item *item);

        bool playerIsAwareOfItem(PlayerData *playerData, Item *item);

    };
}
