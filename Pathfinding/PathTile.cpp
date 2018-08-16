//
// Created by connor on 8/4/18.
//

#include "PathTile.h"
#include "../MathUtils.h"
#include "AStarPathfinder.h"

namespace DsprGameServer
{
    PathTile::PathTile(int x, int y, int heat)
    {
        this->x = x;
        this->y = y;
        this->heat = heat;
    }

    int PathTile::getTileId() const
    {
        return getTileId(this->x, this->y);
    }

    int PathTile::getTileId(int x, int y) {
        return (y*AStarPathfinder::getMapWidth()) + x;
    }
}