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
        PathTile(int x, int y);
        void setNextTile(int tileId);
        int getTileId() const;
        inline bool operator == (PathTile const& lhs, PathTile const& rhs);
    private:
        int x;
        int y;
        int nextTileId = -1;
    };

    struct PathTileHasher
    {
        size_t operator() (const PathTile &tile) const
        {
            return (size_t) tile.getTileId();
        }
    };
}