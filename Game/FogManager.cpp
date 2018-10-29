//
// Created by connor on 9/7/18.
//

#include "FogManager.h"
#include "Game.h"
#include "TileManager.h"
#include "Circle/CircleCache.h"
#include "../PrimIsoGrid.h"
#include "Tribe.h"

namespace DsprGameServer
{
    FogManager::FogManager(Game *game)
    {
        this->game = game;
    }

    FogManager::~FogManager()
    {

    }

    void FogManager::addTribe(DsprGameServer::Tribe *newTribe)
    {
        auto newGrid = new PrimIsoGrid<int>();
        newGrid->initialize(this->game->tileManager->width * 2, this->game->tileManager->height * 2);
        this->fogGridMap.emplace(std::make_pair(newTribe, newGrid));
    }

    void FogManager::revealFog(Tribe *tribe, int x, int y, int radius)
    {
        this->updateFog(tribe, x, y, radius, true);
    }

    void FogManager::conceilFog(Tribe *tribe, int x, int y, int radius)
    {
        this->updateFog(tribe, x, y, radius, false);
    }

    void FogManager::updateFog(Tribe *tribe, int centerX, int centerY, int radius, bool reveal)
    {
        if (tribe == nullptr)return;
        if (tribe->isNeutral())return;

        auto fogGrid = this->fogGridMap.at(tribe);

        auto sightCircle = CircleCache::get().getCircle(radius);

        int inc = reveal ? 1 : -1;

        for(auto circleCoord : sightCircle->coordList){
            int x = centerX + circleCoord->x;
            int y = centerY + circleCoord->y;

            if (!fogGrid->insideGrid(x,y)) continue;

            int currentFogAmount = fogGrid->get(x,y);
            if (reveal && currentFogAmount == 0)
            {
                currentFogAmount += 2;
                this->game->tileManager->sendTileToPlayer(x,y, tribe->playerData);
            }
            else
            {
                currentFogAmount += inc;
            }

            fogGrid->set(x,y,currentFogAmount);
        }
    }

    bool FogManager::tileIsInShroud(Tribe* tribe, int x, int y)
    {
        return this->getFogAmount(tribe, x, y) == 0;
    }

    bool FogManager::tileIsInFog(Tribe* tribe, int x, int y)
    {
        return this->getFogAmount(tribe, x, y) == 1;
    }

    bool FogManager::tileIsClear(Tribe* tribe, int x, int y)
    {
        return this->getFogAmount(tribe, x, y) > 1;
    }

    int FogManager::getFogAmount(Tribe *tribe, int x, int y) {
        auto fogGrid = this->fogGridMap.at(tribe);

        return fogGrid->get(x,y);
    }

    void FogManager::forEachFogTile(Tribe* tribe, const std::function<void(int fogAmount, int x, int y)>& elFunc){
        if (tribe == nullptr)return;
        auto fogGrid = this->fogGridMap.at(tribe);
        fogGrid->forEachElement(elFunc);
    }
}

