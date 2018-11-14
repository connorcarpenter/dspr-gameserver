#pragma once

//
// Created by connor on 8/27/18.
//

#include <set>
#include "Game.h"

namespace DsprGameServer
{
    class PlayerData;

    class Tribe
    {
    public:
        Tribe(Game *game, int color);
        ~Tribe();
        void setEnemy(Tribe* otherTribe);
        int index = -1;
        PlayerData* playerData = nullptr;
        bool isNeutral();
        bool playable = false;
    private:
        std::set<Tribe*> enemies;
        Game *game = nullptr;
    };
}
