#pragma once

//
// Created by connor on 9/7/18.
//

#include <map>
#include <functional>
#include "VisionProfile.h"

namespace DsprGameServer
{
    class Tribe;
    class Game;
    template <class T> class PrimIsoGrid;

    class FogManager
    {

    public:

        FogManager(Game *game);
        ~FogManager();

        void addTribe(Tribe* newTribe);
        void removeTribe(Tribe* tribe);

        void revealFog(Tribe* tribe, int x, int y, int radius);
        void conceilFog(Tribe* tribe, int x, int y, int radius);
        bool tileIsClear(Tribe* tribe, int x, int y);
        bool tileIsInFog(Tribe* tribe, int x, int y);
        bool tileIsInShroud(Tribe* tribe, int x, int y);

        void forEachFogTile(Tribe *tribe, const std::function<void(int, int, int)>& elFunc);

    private:

        std::map<VisionProfile*, PrimIsoGrid<int>*> fogGridMap;
        Game* game = nullptr;

        void updateFog(Tribe* tribe, int x, int y, int radius, bool reveal);
        int getFogAmount(Tribe* tribe, int x, int y);


        void sendTileToPlayers(int x, int y, VisionProfile *pProfile);
    };
}