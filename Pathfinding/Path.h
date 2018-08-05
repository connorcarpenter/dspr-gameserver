#pragma once

//
// Created by connor on 8/4/18.
//

#include "PathTile.h"

#include <unordered_map>

namespace DsprGameServer
{
    class Path
    {
    public:
        Path(bool pathFound);
        ~Path();
        bool pathFound = true;
        std::unordered_map<int, PathTile*> startTiles;
        std::unordered_map<int, PathTile*> pathTiles;
        std::unordered_map<int, PathTile*> endTiles;
    };
}
