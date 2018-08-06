//
// Created by connor on 8/4/18.
//

#include "Path.h"

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
            delete tile.second;
        for(auto tile : endTiles)
            delete tile.second;
    }

    PathTile *Path::getStartTile(int x, int y) {
        int tileId = PathTile::getTileId(x,y);
        if (startTiles.count(tileId) == 0) return nullptr;
        return startTiles.at(tileId);
    }

    PathTile *Path::getPathTile(int id) {
        if (pathTiles.count(id) == 0) return nullptr;
        return pathTiles.at(id);
    }

    PathTile *Path::getEndTile(int id) {
        if (endTiles.count(id) == 0) return nullptr;
        return endTiles.at(id);
    }
}