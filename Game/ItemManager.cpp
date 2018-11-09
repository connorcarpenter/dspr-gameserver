//
// Created by connor on 11/8/18.
//

#include <sstream>
#include "ItemManager.h"
#include "Game.h"
#include "TileManager.h"
#include "../GameServer.h"
#include "../PlayerData.h"

namespace DsprGameServer {

    ItemManager::ItemManager(Game *game) {
        this->game = game;

        this->itemGrid = new PtrIsoGrid<Item*>();
        this->itemGrid->initialize(this->game->tileManager->width * 2, this->game->tileManager->height * 2);
    }

    ItemManager::~ItemManager() {
        for(const auto& itemPair : itemMap)
        {
            Item* item = itemPair.second;
            delete item;
        }

        delete itemGrid;

        for(const auto& mapPair : this->playerToItemsAwareOfMap)
        {
            std::set<Item*>* set = mapPair.second;
            delete set;
        }
    }

    void ItemManager::initializeFirstItems() {
        this->createItem((3) * 2, (10) * 2, 0, 0);
        this->createItem((4) * 2, (10) * 2, 2, 1);
        this->createItem((5) * 2, (10) * 2, 3, 2);
        this->createItem((6) * 2, (10) * 2, 4, 3);
        this->createItem((4) * 2, (11) * 2, 5, 4);
    }
    
    void ItemManager::createItem(int x, int y, int templateIndex, int id) {
        Item* newItem = new Item(this->game, id, x, y, templateIndex);
        this->itemMap.insert(std::pair<int, Item*>(newItem->id, newItem));
    }

    void ItemManager::addItemToGrid(Item *item) {
        this->setItemInGrid(item->position->x, item->position->y, item);
    }

    void ItemManager::setItemInGrid(int x, int y, Item *item) {
        this->itemGrid->set(x,y, item);
    }

    void ItemManager::sendItemUpdates(PlayerData *playerData)
    {
        for(const auto& itemPair : itemMap)
        {
            Item* item = itemPair.second;
            bool playerIsAwareOfItem = this->playerIsAwareOfItem(playerData, item);

            if (playerIsAwareOfItem)
            {
                if (!item->isVisibleToTribe(playerData->getTribe()))
                {
                    this->makePlayerUnawareOfItem(playerData, item);
                }
            }
            else
            {
                if (item->isVisibleToTribe(playerData->getTribe()))
                {
                    this->makePlayerAwareOfItem(playerData, item);
                }
            }
        }
    }
    
    void ItemManager::makePlayerAwareOfItem(PlayerData *playerData, Item *item) {

        std::stringstream msg;
        msg << "item/1.0/create|" << item->id << "," << item->position->x << "," << item->position->y << ","
            << item->index << "\r\n";

        GameServer::get().queueMessage(playerData, msg.str());

        std::set<Item*>* itemSet = this->playerToItemsAwareOfMap.at(playerData);
        itemSet->emplace(item);
    }

    void ItemManager::makePlayerUnawareOfItem(PlayerData *playerData, Item *item) {

        std::stringstream msg;
        msg << "item/1.0/delete|" << item->id << "|0" << "\r\n";
        GameServer::get().queueMessage(playerData, msg.str());

        std::set<Item*>* itemSet = this->playerToItemsAwareOfMap.at(playerData);
        itemSet->erase(item);
    }

    bool ItemManager::playerIsAwareOfItem(PlayerData *playerData, Item *item) {
        auto playerItemAwareSet = this->playerToItemsAwareOfMap.at(playerData);
        return playerItemAwareSet->count(item) != 0;
    }

    void ItemManager::addPlayer(PlayerData *playerData) {
        this->playerToItemsAwareOfMap.emplace(std::make_pair(playerData, new std::set<Item*>()));
    }

    void ItemManager::removePlayer(PlayerData *playerData) {
        if (this->playerToItemsAwareOfMap.count(playerData) <= 0) return;
        auto itemSet = this->playerToItemsAwareOfMap.at(playerData);
        delete itemSet;
        this->playerToItemsAwareOfMap.erase(playerData);
    }

    void ItemManager::initSendAllItems(PlayerData *playerData) {
        for(const auto& itemPair : itemMap)
        {
            int id = itemPair.first;
            Item* item = itemPair.second;

            if (item->isVisibleToTribe(playerData->getTribe()))
            {
                this->makePlayerAwareOfItem(playerData, item);
            }
        }
    }
}