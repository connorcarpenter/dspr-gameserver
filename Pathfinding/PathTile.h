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
        static int getTileId(int x, int y);
        float getF()const;
        float getH(int endX, int endY);

        int x;
        int y;
        float g = 0;
        float h = 0;
        int previousTileId = -1;

        int getNextTile();

    private:
        int nextTileId = -1;


    };
}