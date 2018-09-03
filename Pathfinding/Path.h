#pragma once

//
// Created by connor on 8/4/18.
//

#include <unordered_map>
#include <list>
#include "../Game/Game.h"

namespace DsprGameServer
{
    class PathTile;

    class Path
    {
    public:
        Path(Game *game, int targetX, int targetY);
        ~Path();
        PathTile* getTile(int x, int y);
        void addTile(PathTile* tile);

        PathTile* getEndTile(int x, int y);
        void addEndTile(PathTile *tile);

        int targetX;
        int targetY;
        bool foundEnd = false;

    private:
        std::unordered_map<int, PathTile*> tiles;
        std::unordered_map<int, PathTile*> endTiles;
        Game *game = nullptr;
    };
}
