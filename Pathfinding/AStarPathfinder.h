#pragma once

//
// Created by connor on 8/4/18.
//

#include <queue>
#include <unordered_map>
#include <list>
#include <bits/unordered_map.h>
#include "../Game/TileManager.h"
#include "Path.h"

namespace DsprGameServer
{
    class Game;
    class PathNode;
    class PathNodeComparator;

    class AStarPathfinder
    {
    public:
        AStarPathfinder(Game *game);

        std::shared_ptr<Path> findPath(const std::list<std::pair<int, int>> &unitPositions, int targetX, int targetY);

        static void setMapWidth(int width);
        static int getMapWidth();
        static int mapWidth;
    private:
        std::list<PathNode*>* getNeighbors(PathNode* parent, int targetX, int targetY);

        void tryAddNeighbor(std::list<PathNode*>* neighborList, PathNode* parentTile,
                       int tileX, int y, int targetX, int targetY, float cost);

        void cleanUp(std::list<PathNode *> *nodes, std::unordered_map<int, PathNode *> *map,
                     std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>* heap, std::unordered_map<int, PathNode *> *map2);

        void addToPath(std::shared_ptr<Path> path, int startX, int startY, int targetX, int targetY);

        const float straightCost = 2;
        const float diagonalCost = 3;
        const int disToEndUnits = 10;

        Game* game = nullptr;
    };
}