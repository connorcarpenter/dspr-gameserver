//
// Created by connor on 8/4/18.
//

#include "Path.h"
#include "PathTile.h"

namespace DsprGameServer
{
    Path::Path(bool pathFound)
    {
        this->pathFound = pathFound;
    }

    Path::~Path()
    {
        for(auto tile : startTiles)
            delete tile.second;
        for(auto tile : pathTiles)
            delete tile;
        for(auto tile : endTiles)
            delete tile.second;
    }

    PathTile* Path::getStartTile(int x, int y)
    {
        int tileId = PathTile::getTileId(x,y);
        if (startTiles.count(tileId) == 0) return nullptr;
        return startTiles.at(tileId);
    }

    bool Path::tileIsEnd(PathTile *tile)
    {
        return (endTiles.count(tile->getTileId()) != 0);
    }

    void Path::addStartTile(PathTile* tile)
    {
        startTiles.emplace(tile->getTileId(), tile);
    }

    void Path::addEndTile(PathTile* tile)
    {
        endTiles.emplace(tile->getTileId(), tile);
    }

    void Path::addTargetTile(PathTile* tile)
    {
        targetTile = tile;
    }

    void Path::addPathTile(PathTile* tile)
    {
        pathTiles.push_back(tile);
    }

    PathTile *Path::getTargetTile() {
        return targetTile;
    }
}