//
// Created by connor on 8/4/18.
//

#include "PathTile.h"
#include "../MathUtils.h"
#include "AStarPathfinder.h"

namespace DsprGameServer
{
    PathTile::PathTile(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    PathTile::PathTile(int x, int y, int endX, int endY) {
        this->x = x;
        this->y = y;
        this->h = getH(endX, endY);
    }

    PathTile::PathTile(int x, int y, const PathTile *parent, float parDis, int tarX, int tarY) {
        this->x = x;
        this->y = y;
        this->g = parent->g + parDis;
        this->setPreviousTile(parent->getTileId());
        this->h = getH(tarX, tarY);
    }

    PathTile::PathTile(PathTile *other)
    {
        this->x = other->x;
        this->y = other->y;
        this->g = other->g;
        this->h = other->h;
        this->previousTileId = other->previousTileId;
        this->nextTileId = other->nextTileId;
    }

    void PathTile::setNextTile(int tileId)
    {
        this->nextTileId = tileId;
    }

//    int PathTile::getTileId()
//    {
//        return getTileId(this->x, this->y);
//    }

    int PathTile::getTileId() const
    {
        return getTileId(this->x, this->y);
    }

    int PathTile::getTileId(int x, int y) {
        return (y*AStarPathfinder::getMapWidth()) + x;
    }

    void PathTile::setPreviousTile(int tileId)
    {
        this->previousTileId = tileId;
    }

    float PathTile::getF()const {
        return this->g + this->h;
    }

    float PathTile::getH(int endX, int endY) {

        int d_max = MathUtils::Max(MathUtils::Abs(this->x - endX), MathUtils::Abs(this->y - endY));
        int d_min = MathUtils::Min(MathUtils::Abs(this->x - endX), MathUtils::Abs(this->y - endY));
        return (1.414f * d_min) + (d_max - d_min);
    }

    int PathTile::getNextTile() {
        return this->nextTileId;
    }


}