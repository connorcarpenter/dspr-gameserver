//
// Created by connor on 8/27/18.
//

#include "Tribe.h"

#include <utility>
#include "Game.h"
#include "EconomyManager.h"
#include "TribeManager.h"

namespace DsprGameServer
{
    Tribe::Tribe(Game *game, int index) {
        this->index = index;
        this->game = game;
        game->economyManager->addTribe(this);
        this->visionProfile = std::make_shared<VisionProfile>();
        this->visionProfile->addTribe(this);
    }

    Tribe::Tribe(Game *game, int index, std::shared_ptr<VisionProfile> visionProfile) {
        this->index = index;
        this->game = game;
        game->economyManager->addTribe(this);
        this->visionProfile = std::move(visionProfile);
        this->visionProfile->addTribe(this);
    }

    Tribe::~Tribe()
    {
        game->economyManager->removeTribe(this);
    }

    void Tribe::setEnemy(Tribe* otherTribe) {
        this->enemies.insert(otherTribe);
    }

    void Tribe::setAlly(Tribe* otherTribe) {
        this->allies.insert(otherTribe);
    }

    bool Tribe::isNeutral() {
        return game->tribeManager->neutralTribe->index == index;
    }

    VisionProfile* Tribe::getVisionProfile() {
        return this->visionProfile.get();
    }

    bool Tribe::isEnemiesWith(Tribe *otherTribe) {
        return this->enemies.count(otherTribe) != 0;
    }

    bool Tribe::isAlliesWith(Tribe *otherTribe) {
        return this->allies.count(otherTribe) != 0;
    }
}