#pragma once

//
// Created by connor on 8/27/18.
//

#include <set>
#include "Game.h"
#include "VisionProfile.h"

namespace DsprGameServer
{
    class PlayerData;
    class VisionProfile;

    class Tribe
    {
    public:
        Tribe(Game *game, int color);

        Tribe(Game *game, int index, std::shared_ptr<VisionProfile> visionProfile);

        ~Tribe();
        void setEnemy(Tribe* otherTribe);
        void setAlly(Tribe *otherTribe);
        int index = 0;
        PlayerData* playerData = nullptr;
        bool isNeutral();
        bool playable = false;

        VisionProfile *getVisionProfile();

        bool isEnemiesWith(Tribe *otherTribe);
        bool isAlliesWith(Tribe *otherTribe);

    private:
        std::set<Tribe*> enemies;
        std::set<Tribe*> allies;
        Game *game = nullptr;
        std::shared_ptr<VisionProfile> visionProfile = nullptr;


    };
}
