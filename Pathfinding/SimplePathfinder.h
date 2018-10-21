#pragma once

//
// Created by connor on 8/31/18.
//

#include "../Game/Game.h"

namespace DsprGameServer {

    class Point;
    class Unit;
    class Path;
    class PathNode;
    class PathNodeComparator;

    class SimplePathfinder {

    public:
        SimplePathfinder(DsprGameServer::Game *game);

        Point* findNextPosition(Unit* unit, std::shared_ptr<DsprGameServer::Path> path);

        Game *game = nullptr;

        void cleanUp(std::list<DsprGameServer::PathNode *> *nodes, std::unordered_map<int, DsprGameServer::PathNode *> *map,
                 std::priority_queue<DsprGameServer::PathNode *, std::vector<DsprGameServer::PathNode *>, DsprGameServer::PathNodeComparator> *heap,
                 std::unordered_map<int, DsprGameServer::PathNode *> *map2);

        std::list<PathNode *> *getNeighbors(PathNode *parent, int targetX, int targetY, Unit *unit);

        void tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj, float cost,
                                    int targetX, int targetY, Unit *unit);

    private:
        const float straightCost = 2;
        const float diagonalCost = 3;
        const int disToEndUnits = 10;
        const int maxNodeCost = (int) (straightCost * diagonalCost) * 3;
    };
}