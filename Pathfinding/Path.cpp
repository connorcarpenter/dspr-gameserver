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
}