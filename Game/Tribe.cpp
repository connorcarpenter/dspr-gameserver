//
// Created by connor on 8/27/18.
//

#include "Tribe.h"
#include "Game.h"
#include "EconomyManager.h"
#include "TribeManager.h"

namespace DsprGameServer
{
    Tribe::Tribe(Game *game, int index) {
        this->index = index;
        this->game = game;
        game->economyManager->addTribe(this);
    }

    Tribe::~Tribe()
    {
        game->economyManager->removeTribe(this);
    }

    void Tribe::setEnemy(Tribe* otherTribe) {
        this->enemies.insert(otherTribe);
    }

    bool Tribe::isNeutral() {
        return game->tribeManager->neutralTribe->index == index;
    }
}