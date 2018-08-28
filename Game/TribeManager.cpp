//
// Created by connor on 8/27/18.
//

#include "TribeManager.h"

namespace DsprGameServer
{

    TribeManager::TribeManager(Game *game) {
        this->game = game;

        this->tribeA = new Tribe(0);
        this->tribeB = new Tribe(1);
        this->tribeA->setEnemy(this->tribeB);
        this->tribeB->setEnemy(this->tribeA);
        tribeSet.insert(this->tribeA);
        tribeSet.insert(this->tribeB);
    }

    TribeManager::~TribeManager() {
        for(Tribe* tribe : tribeSet)
        {
            delete tribe;
        }
    }
}