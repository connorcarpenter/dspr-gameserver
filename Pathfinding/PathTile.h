#pragma once

//
// Created by connor on 8/4/18.
//

#include <cstddef>
#include "PathNode.h"

namespace DsprGameServer
{
    class PathTile
    {
    public:
        PathTile(int x, int y, int heat);

        int getTileId() const;
        static int getTileId(int x, int y);

        int x;
        int y;
        PathTile* nextTile = nullptr;

        int heat;

    };
}