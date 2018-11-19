//
// Created by connor on 11/2/18.
//

#include "EconomyManager.h"
#include "../GameServer.h"
#include <sstream>
#include "TribeManager.h"

namespace DsprGameServer
{
    EconomyManager::EconomyManager(Game *pGame) {
        this->game = pGame;
    }

    EconomyManager::~EconomyManager() {
        for(const auto& mapPair : this->playerToPopMap)
        {
            auto var = mapPair.second;
            delete var;
        }

        for(const auto& mapPair : this->playerToPopMaxMap)
        {
            auto var = mapPair.second;
            delete var;
        }

        for(const auto& mapPair : this->playerToManaMap)
        {
            auto var = mapPair.second;
            delete var;
        }
    }

    void EconomyManager::addTribe(Tribe *tribe) {
        auto syncedVar = new Synced<Int>("pop", new Int(3));
        syncedVar->dirty();
        this->playerToPopMap.emplace(tribe, syncedVar);

        syncedVar = new Synced<Int>("popMax", new Int(25));
        syncedVar->dirty();
        this->playerToPopMaxMap.emplace(tribe, syncedVar);

        syncedVar = new Synced<Int>("mana", new Int(0));
        syncedVar->dirty();
        this->playerToManaMap.emplace(tribe, syncedVar);
    }

    void EconomyManager::removeTribe(Tribe *tribe) {
        if (this->playerToPopMap.count(tribe) <= 0) return;
        auto popVar = this->playerToPopMap.at(tribe);
        delete popVar;
        this->playerToPopMap.erase(tribe);

        auto popMaxVar = this->playerToPopMaxMap.at(tribe);
        delete popMaxVar;
        this->playerToPopMaxMap.erase(tribe);

        auto manaVar = this->playerToManaMap.at(tribe);
        delete manaVar;
        this->playerToManaMap.erase(tribe);
    }

    void EconomyManager::sendUpdates(PlayerData* player, bool overrideDirty) {

        Tribe* tribe = this->game->tribeManager->getTribeFromPlayer(player);

        if (!overrideDirty && !this->anyVarIsDirty(tribe)) return;
        std::stringstream msg;
        msg << "economy/1.0/update|";
        bool firstVar = true;

        auto popVar = this->playerToPopMap.at(tribe);
        if (overrideDirty || popVar->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << popVar->serialize();
        }

        auto popMaxVar = this->playerToPopMaxMap.at(tribe);
        if (overrideDirty || popMaxVar->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << popMaxVar->serialize();
        }

        auto manaVar = this->playerToManaMap.at(tribe);
        if (overrideDirty || manaVar->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << manaVar->serialize();
        }

        //next synced variable should follow this format
//        auto manaVar = this->playerToManaMap.at(player);
//        if (overrideDirty || manaVar->isDirty())
//        {
//            if (firstVar) { firstVar = false; } else { msg << "&"; }
//            msg << manaVar->serialize();
//        }

        msg << "\r\n";
        GameServer::get().queueMessage(player, msg.str());
    }

    void EconomyManager::clean() {

        //for(const auto& mapPair : this->playerToPopMap) mapPair.second->clean();

        //for(const auto& mapPair : this->playerToPopMaxMap) mapPair.second->clean();

        //for(const auto& mapPair : this->playerToManaMap) mapPair.second->clean();
        //more synced vars here
    }

    bool EconomyManager::anyVarIsDirty(Tribe* tribe) {
        if (this->playerToManaMap.at(tribe)->isDirty()) return true;
        if (this->playerToPopMap.at(tribe)->isDirty()) return true;
        if (this->playerToPopMaxMap.at(tribe)->isDirty()) return true;
        //more synced vars here
        return false;
    }

    void EconomyManager::setMana(Tribe* tribe, int value) {
        this->playerToManaMap.at(tribe)->dirtyObj()->Set(value);
    }

    void EconomyManager::setManaClean(Tribe *tribe, int value) {
        this->playerToManaMap.at(tribe)->obj()->Set(value);
    }

    int EconomyManager::getMana(Tribe* tribe) {
        return this->playerToManaMap.at(tribe)->obj()->Get();
    }
}