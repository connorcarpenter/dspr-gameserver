#pragma once

//
// Created by connor on 8/4/18.
//

#include "PathTile.h"

#include <unordered_set>

namespace DsprGameServer
{
    class Path
    {
        std::unordered_set<PathTile, PathTileHasher> startTiles;
        std::unordered_set<PathTile, PathTileHasher> pathTiles;
        std::unordered_set<PathTile, PathTileHasher> endTiles;
    };
}
