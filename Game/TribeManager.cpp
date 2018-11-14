//
// Created by connor on 8/27/18.
//

#include "TribeManager.h"
#include "FogManager.h"
#include "../PlayerData.h"

namespace DsprGameServer
{
    TribeManager::TribeManager(Game *game)
    {
        this->game = game;

        this->neutral = new Tribe(game, -1);
        this->tribeA = new Tribe(game, 0);
        this->tribeB = new Tribe(game, 1);
        this->tribeCreep = new Tribe(game, 2);

        tribeSet.insert(this->tribeA);
        this->tribeA->setEnemy(this->tribeB);
        this->tribeA->setEnemy(this->tribeCreep);
        this->tribeA->playable = true;
        this->game->fogManager->addTribe(this->tribeA);

        tribeSet.insert(this->tribeB);
        this->tribeB->setEnemy(this->tribeA);
        this->tribeB->setEnemy(this->tribeCreep);
        this->tribeB->playable = true;
        this->game->fogManager->addTribe(this->tribeB);

        tribeSet.insert(this->tribeCreep);
        this->tribeCreep->setEnemy(this->tribeA);
        this->tribeCreep->setEnemy(this->tribeB);
        this->game->fogManager->addTribe(this->tribeCreep);
    }

    TribeManager::~TribeManager()
    {
        for(Tribe* tribe : tribeSet)
        {
            delete tribe;
        }

        delete this->neutral;
    }

    Tribe *TribeManager::getFreeTribe() {
        for (auto tribe: tribeSet){
            if (tribe->playerData == nullptr && tribe->playable){
                return tribe;
            }
        }

        return nullptr;
    }

    void TribeManager::assignTribeToPlayer(Tribe *tribe, PlayerData *playerData) {
        tribe->playerData = playerData;
        playerData->setTribe(tribe);
        this->playerToTribeMap.emplace(std::make_pair(playerData, tribe));
    }

    void TribeManager::freeTribeFromPlayer(Tribe *tribe, PlayerData *playerData) {
        tribe->playerData = nullptr;
        playerData->setTribe(nullptr);
        this->playerToTribeMap.erase(playerData);
    }

    Tribe *TribeManager::getTribeFromPlayer(PlayerData *playerData) {
        return this->playerToTribeMap.at(playerData);
    }
}