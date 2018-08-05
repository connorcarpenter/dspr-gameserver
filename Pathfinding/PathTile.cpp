//
// Created by connor on 8/4/18.
//

#include "PathTile.h"
#include "Pathfinder.h"

namespace DsprGameServer
{
    PathTile::PathTile(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void PathTile::setNextTile(int tileId)
    {
        this->nextTileId = tileId;
    }

    int PathTile::getTileId() const
    {
        return (this->y*Pathfinder::getMapWidth()) + this->x;
    }

    bool PathTile::operator==(PathTile const &lhs, PathTile const &rhs) {
        return (lhs.x == rhs.x && lhs.y == rhs.y);
    }
}