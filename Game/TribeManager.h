#pragma once

//
// Created by connor on 8/27/18.
//

#include <set>
#include <map>
#include "Game.h"
#include "Tribe.h"

namespace DsprGameServer
{
    class TribeManager
    {
    public:
        TribeManager(Game* game);
        ~TribeManager();

        Tribe* tribeA = nullptr;
        Tribe* tribeB = nullptr;
        Tribe* neutral = nullptr;

        Tribe *getFreeTribe();

        void assignTribeToPlayer(Tribe *tribe, PlayerData *playerData);

        void freeTribeFromPlayer(Tribe *tribe, PlayerData *playerData);

    private:
        DsprGameServer::Game* game = nullptr;
        std::set<Tribe*> tribeSet;
        std::map<PlayerData*, Tribe*> playerToTribeMap;
    };
}