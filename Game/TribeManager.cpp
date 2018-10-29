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

        this->neutral = new Tribe(-1);
        this->tribeA = new Tribe(0);
        this->tribeB = new Tribe(1);

        tribeSet.insert(this->tribeA);
        this->tribeA->setEnemy(this->tribeB);
        this->game->fogManager->addTribe(this->tribeA);

        tribeSet.insert(this->tribeB);
        this->tribeB->setEnemy(this->tribeA);
        this->game->fogManager->addTribe(this->tribeB);
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
            if (tribe->playerData == nullptr){
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
}