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

        PathTile(int x, int y, const PathTile *parent, float parDis, int tarX, int tarY);

        PathTile(int x, int y, int endX, int endY);

        PathTile(PathTile *other);

        void setNextTile(int tileId);
        void setPreviousTile(int tileId);
        int getTileId() const;
        float getF()const;
        float getH(int endX, int endY);

        int x;
        int y;
        float g = 0;
        float h = 0;
        int previousTileId = -1;
    private:
        int nextTileId = -1;
    };

    struct PathTileHasher
    {
        size_t operator() (const PathTile &tile) const
        {
            return (size_t) tile.getTileId();
        }

        size_t operator() (const PathTile* tile) const
        {
            return (size_t) tile->getTileId();
        }
    };

    class PathTileComparator
    {
    public:
        int operator() (const PathTile& p1, const PathTile& p2)
        {
            return p1.getF() > p2.getF();
        }
    };

    class PathTilePred
    {
    public:
        bool operator() (const PathTile& p1, const PathTile& p2) const
        {
            return p1.getTileId() == p2.getTileId();
        }
        bool operator() (const PathTile* p1, const PathTile* p2) const
        {
            return p1->getTileId() == p1->getTileId();
        }
    };
}