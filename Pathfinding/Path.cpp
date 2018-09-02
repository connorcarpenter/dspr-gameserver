//
// Created by connor on 8/4/18.
//

#include "Path.h"
#include "PathTile.h"

namespace DsprGameServer
{
    Path::Path(int targetX, int targetY)
    {
        this->targetX = targetX;
        this->targetY = targetY;
    }

    Path::~Path()
    {
        for(auto tile : tiles)
            delete tile.second;

        for(auto tile : endTiles)
            delete tile.second;
    }

    PathTile* Path::getTile(int x, int y)
    {
        int tileId = PathTile::getTileId(x,y);
        if (tiles.count(tileId) == 0) return nullptr;
        return tiles.at(tileId);
    }

    void Path::addTile(PathTile *tile)
    {
        tiles.emplace(tile->getTileId(), tile);
    }

    PathTile* Path::getEndTile(int x, int y)
    {
        int tileId = PathTile::getTileId(x,y);
        if (endTiles.count(tileId) == 0) return nullptr;
        return endTiles.at(tileId);
    }

    void Path::addEndTile(PathTile *tile)
    {
        endTiles.emplace(tile->getTileId(), tile);
    }
}