//
// Created by connor on 8/27/18.
//

#include "TribeManager.h"
#include "FogManager.h"
#include "../PlayerData.h"
#include "VisionProfile.h"
#include "../DsprMessage/ToClientMsg.h"
#include "../GameServer.h"

namespace DsprGameServer
{
    TribeManager::TribeManager(Game *game)
    {
        this->game = game;

        auto playerVisionProfile = std::make_shared<VisionProfile>();

        this->neutralTribe = new Tribe(game, 0);
        this->tribeA = new Tribe(game, 1, playerVisionProfile);
        this->tribeB = new Tribe(game, 2, playerVisionProfile);
        this->tribeCreep = new Tribe(game, 3);

        tribeSet.insert(this->tribeA);
        this->tribeA->setEnemy(this->tribeCreep);
        this->tribeA->setAlly(this->tribeB);
        this->tribeA->playable = true;
        this->game->fogManager->addTribe(this->tribeA);

        tribeSet.insert(this->tribeB);
        this->tribeB->setEnemy(this->tribeCreep);
        this->tribeB->setAlly(this->tribeA);
        this->tribeB->playable = true;
        this->game->fogManager->addTribe(this->tribeB);

        tribeSet.insert(this->tribeCreep);
        this->tribeCreep->playable = false;
        this->tribeCreep->setEnemy(this->tribeA);
        this->tribeCreep->setEnemy(this->tribeB);
        this->game->fogManager->addTribe(this->tribeCreep);

        tribeSet.insert(this->neutralTribe);
        this->neutralTribe->playable = false;
    }

    TribeManager::~TribeManager()
    {
        for(Tribe* tribe : tribeSet)
        {
            delete tribe;
        }
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

    bool TribeManager::areTribesEnemies(Tribe *tribeA, Tribe *tribeB) {
        if (tribeA->isEnemiesWith(tribeB))return true;
        if (tribeB->isEnemiesWith(tribeA))return true;
        return false;
    }

    void TribeManager::sendTribeData(PlayerData *playerData)
    {
        Tribe* playerTribe = playerData->getTribe();
        for(Tribe* tribe : tribeSet)
        {
            DsprMessage::TribeSetMsgV1 tribeSetMsgV1;
            tribeSetMsgV1.tribeIndex.set(tribe->index);
            if (playerTribe == tribe)
            {
                tribeSetMsgV1.status.set((unsigned int) TribeStatus::Player);
            }
            else
            if (playerTribe->isAlliesWith(tribe))
            {
                tribeSetMsgV1.status.set((unsigned int) TribeStatus::Ally);
            }
            else
            if (playerTribe->isEnemiesWith(tribe))
            {
                tribeSetMsgV1.status.set((unsigned int) TribeStatus::Enemy);
            }
            else
            {
                tribeSetMsgV1.status.set((unsigned int) TribeStatus::Neutral);
            }

            auto clientMsg = tribeSetMsgV1.getToClientMessage();
            auto packedMsg = clientMsg->Pack();
            GameServer::get().queueMessage(playerData, packedMsg);
        }
    }
}