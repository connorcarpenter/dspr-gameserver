//
// Created by connor on 8/27/18.
//

#include "TribeManager.h"
#include "FogManager.h"

namespace DsprGameServer
{
    TribeManager::TribeManager(Game *game)
    {
        this->game = game;

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
    }
}