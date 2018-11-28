//
// Created by connor on 11/8/18.
//

#include <sstream>
#include "ItemManager.h"
#include "../Game.h"
#include "../TileManager.h"
#include "../../GameServer.h"
#include "../../PlayerData.h"
#include "ItemTemplateCatalog.h"
#include "../../DsprMessage/ToClientMsg.h"

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
//        this->createItem((3) * 2, (10) * 2, this->game->itemTemplateCatalog->sling);
//        this->createItem((4) * 2, (10) * 2, this->game->itemTemplateCatalog->club);
//        this->createItem((5) * 2, (10) * 2, this->game->itemTemplateCatalog->helmet);
//        this->createItem((6) * 2, (10) * 2, this->game->itemTemplateCatalog->armor);
//        this->createItem((4) * 2, (11) * 2, this->game->itemTemplateCatalog->shield);
    }
    
    void ItemManager::createItem(int x, int y, ItemTemplate *itemTemplate) {
        if (itemTemplate == nullptr)return;
        Item* newItem = new Item(this->game, getFreeItemId(), x, y, itemTemplate);
        this->itemMap.insert(std::pair<int, Item*>(newItem->id, newItem));
    }

    int ItemManager::getFreeItemId() {
        if (!unusedIds.empty())
        {
            auto freeId = unusedIds.front();
            unusedIds.pop();
            return freeId;
        }

        return (int) itemMap.size();
    }

    void ItemManager::addItemToGrid(Item *item) {
        this->setItemInGrid(item->position->x, item->position->y, item);
    }

    void ItemManager::setItemInGrid(int x, int y, Item *item) {
        this->itemGrid->set(x,y, item);
    }

    void ItemManager::dropItemFromInventory(int x, int y, Item* item)
    {
        item->position->x = x;
        item->position->y = y;
        this->addItemToGrid(item);
        item->id = getFreeItemId();
        this->itemMap.insert(std::pair<int, Item*>(item->id, item));

        for( const auto& playerSetPair : this->playerToItemsAwareOfMap ) {
            if (item->isVisibleToTribe(playerSetPair.first->getTribe())) {
                this->makePlayerAwareOfItem(playerSetPair.first, item);
            }
        }
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
    
    void ItemManager::makePlayerAwareOfItem(PlayerData *playerData, Item *item)
    {
        DsprMessage::ItemCreateMsgV1 itemCreateMsgV1;
        itemCreateMsgV1.id.set(item->id);
        itemCreateMsgV1.x.set(item->position->x);
        itemCreateMsgV1.y.set(item->position->y);
        itemCreateMsgV1.templateIndex.set(item->itemTemplate->index);
        auto clientMsg = itemCreateMsgV1.getToClientMessage();
        auto packedMsg = clientMsg->Pack();
        GameServer::get().queueMessage(playerData, packedMsg);

        std::set<Item*>* itemSet = this->playerToItemsAwareOfMap.at(playerData);
        itemSet->emplace(item);
    }

    void ItemManager::makePlayerUnawareOfItem(PlayerData *playerData, Item *item)
    {
        DsprMessage::ItemDeleteMsgV1 itemDeleteMsgV1;
        itemDeleteMsgV1.id.set(item->id);
        auto clientMsg = itemDeleteMsgV1.getToClientMessage();
        auto packedMsg = clientMsg->Pack();
        GameServer::get().queueMessage(playerData, packedMsg);

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

    bool ItemManager::itemExists(int itemId) {
        return this->itemMap.count(itemId) != 0;
    }

    Item *ItemManager::getItem(int id) {
        if (this->itemMap.count(id) ==0) return nullptr;
        return this->itemMap.at(id);
    }

    void ItemManager::removeItem(Item *item) {
        this->itemMap.erase(item->id);
        this->itemGrid->set(item->position->x, item->position->y, nullptr);

        for( const auto& playerSetPair : this->playerToItemsAwareOfMap )
        {
            if (playerSetPair.second->count(item) > 0)
                this->makePlayerUnawareOfItem(playerSetPair.first, item);
        }

        unusedIds.push(item->id);
        item->id = -1;
    }

    bool ItemManager::canPlaceItemAtPos(int x, int y) {
        if (this->itemGrid->get(x,y) != nullptr) return false;
        auto tileThere = this->game->tileManager->getTileAt(x,y);
        if (tileThere == nullptr || !tileThere->walkable) return false;
        return true;
    }

    Item *ItemManager::getItemFromGrid(int x, int y) {
        return this->itemGrid->get(x,y);
    }
}