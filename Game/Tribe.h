#pragma once

//
// Created by connor on 8/27/18.
//

#include <set>

namespace DsprGameServer
{
    class PlayerData;

    class Tribe
    {
    public:
        Tribe(int color);
        void setEnemy(Tribe* otherTribe);
        int index = -1;
        PlayerData* playerData = nullptr;
    private:
        std::set<Tribe*> enemies;
    };
}
