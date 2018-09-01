#pragma once

//
// Created by connor on 8/4/18.
//

#include <cstddef>

namespace DsprGameServer
{
    class PathTile
    {
    public:
        PathTile(int x, int y, int disToEnd);

        int getTileId() const;
        static int getTileId(int x, int y);

        int x;
        int y;
        PathTile* nextTile = nullptr;

        int disToEnd;
    };
}