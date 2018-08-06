#pragma once

//
// Created by connor on 8/4/18.
//

#include <unordered_map>
#include <list>

namespace DsprGameServer
{
    class PathTile;

    class Path
    {
    public:
        Path(bool pathFound);
        ~Path();
        bool pathFound = true;
        PathTile* getStartTile(int x, int y);
        bool tileIsEnd(PathTile* tile);
        void addStartTile(PathTile* tile);
        void addEndTile(PathTile* tile);
        void addPathTile(PathTile* tile);
    private:
        std::unordered_map<int, PathTile*> startTiles;
        std::list<PathTile*> pathTiles;
        std::unordered_map<int, PathTile*> endTiles;
    };
}
