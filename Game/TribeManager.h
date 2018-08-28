//
// Created by connor on 8/27/18.
//

#include <set>
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
    private:
        DsprGameServer::Game* game = nullptr;
        std::set<Tribe*> tribeSet;
    };
}