//
// Created by connor on 9/2/18.
//

#include <queue>
#include <unordered_map>
#include <list>
#include <bits/unordered_map.h>
#include "../Game/TileManager.h"
#include "Path.h"

namespace DsprGameServer {
    class Path;
    class PathNode;
    class PathNodeComparator;

    class MoveEndPosFiller {

    public:
        MoveEndPosFiller(DsprGameServer::Game *game);

        void fillEndTiles(std::shared_ptr<DsprGameServer::Path> path, int unitNumber);


    private:
        std::list<PathNode *> *getNeighbors(PathNode *parent, int targetX, int targetY);

        void tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parentTile, int tileX, int y,
                                    int targetX, int targetY, float cost);

        void cleanUp(std::list<PathNode *> *nodes, std::unordered_map<int, PathNode *> *map,
                     std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>* heap, std::unordered_map<int, PathNode *> *map2);

        Game *game = nullptr;

        const float straightCost = 2;
        const float diagonalCost = 3;
    };
}