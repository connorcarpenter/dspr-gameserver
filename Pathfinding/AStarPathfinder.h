#pragma once

//
// Created by connor on 8/4/18.
//

#include <unordered_map>
#include <list>
#include "../TileManager.h"
#include "Path.h"

namespace DsprGameServer
{
    class AStarPathfinder
    {
    public:
        AStarPathfinder(DsprGameServer::TileManager *tileManager);

        std::shared_ptr<Path> findPath(const std::list<std::pair<int,int>>& unitPositions, int unitNumber, int targetX, int targetY);

        static void setMapWidth(int width);
        static int getMapWidth();
        static int mapWidth;
    private:
        TileManager* tileManager = nullptr;
        std::unordered_map<int, PathTile*>* getNeighbors(const PathTile *parentTile, int targetX, int targetY);
        PathTile* findSmallestF(std::unordered_map<int, PathTile*>* list);

    };
}