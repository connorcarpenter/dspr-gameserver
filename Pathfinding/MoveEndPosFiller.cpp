//
// Created by connor on 9/2/18.
//

#include "MoveEndPosFiller.h"
#include <set>
#include "AStarPathfinder.h"
#include "PathTile.h"
#include "PathNode.h"
#include "../Game/UnitManager.h"

namespace DsprGameServer {

    MoveEndPosFiller::MoveEndPosFiller(DsprGameServer::Game *game) {
        this->game = game;
    }

    void MoveEndPosFiller::fillEndTiles(std::shared_ptr<Path> path, int unitNumber)
    {
        int endTileNumber = (unitNumber == 1) ? 1 : unitNumber+2;
        auto closedMap = new std::unordered_map<int, PathNode*>();
        auto openHeap = new std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>();
        auto openMap = new std::unordered_map<int, PathNode*>();
        auto nodes = new std::list<PathNode*>();

        //put first node into openHeap/Map
        auto startNode = new PathNode(path->targetX, path->targetY, nullptr, 0, path->targetX, path->targetY);
        openHeap->push(startNode);
        openMap->emplace(startNode->getId(), startNode);
        nodes->push_back(startNode);

        while(openMap->size() > 0)
        {
            PathNode* currentNode = openHeap->top(); /*and then delete it*/ openHeap->pop();
            openMap->erase(currentNode->getId());

            if (closedMap->size() + 1 < endTileNumber)
            {
                std::list<PathNode*>* neighbors = getNeighbors(currentNode, path->targetX, path->targetY, true);
                for(auto newNode : *neighbors)
                {
                    if (closedMap->count(newNode->getId()) != 0)
                    {
                        delete newNode;
                        continue;
                    }

                    if (openMap->count(newNode->getId()) == 0)
                    {
                        openHeap->push(newNode);
                        openMap->emplace(newNode->getId(), newNode);
                        nodes->push_back(newNode);
                    }
                    else
                    {
                        delete newNode;
                        continue;
                    }
                }

                //clean up neighbor list
                delete neighbors;
            }
            else
            {
                closedMap->emplace(currentNode->getId(), currentNode);
                break;
            }

            closedMap->emplace(currentNode->getId(), currentNode);
        }

        //all our end tiles are in the closedMap, so iterate through that
        for(auto nodePair : *closedMap) {
            auto node = nodePair.second;
            path->addEndTile(new PathTile(node->x, node->y, 0));
        }

        cleanUp(nodes, closedMap, openHeap, openMap);
        return;
    }

    std::list<PathNode *> * MoveEndPosFiller::getNeighbors(PathNode *parent, int targetX, int targetY, bool endFilling)
    {
        auto neighbors = new std::list<PathNode*>();

        tryAddNeighbor(neighbors, parent, 2, 0, targetX, targetY, this->diagonalCost, endFilling);
        tryAddNeighbor(neighbors, parent, -2, 0, targetX, targetY, this->diagonalCost, endFilling);
        tryAddNeighbor(neighbors, parent, 0, 2, targetX, targetY, this->diagonalCost, endFilling);
        tryAddNeighbor(neighbors, parent, 0, -2, targetX, targetY, this->diagonalCost, endFilling);

        tryAddNeighbor(neighbors, parent, 1, 1, targetX, targetY, this->straightCost, endFilling);
        tryAddNeighbor(neighbors, parent, -1, -1, targetX, targetY, this->straightCost, endFilling);
        tryAddNeighbor(neighbors, parent, 1, -1, targetX, targetY, this->straightCost, endFilling);
        tryAddNeighbor(neighbors, parent, -1, 1, targetX, targetY, this->straightCost, endFilling);

        return neighbors;
    }

    void MoveEndPosFiller::tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj, int targetX,
                                         int targetY, float cost, bool endFilling)
    {

        int x = parent->x + xAdj;
        int y = parent->y + yAdj;
        auto tile = this->game->tileManager->getTileAt(x, y);
        if (tile == nullptr) return;
        if (!tile->walkable) return;
        if (endFilling) if (this->game->unitManager->getEndPosAtCoord(x, y)) return;

        auto newPathNode = new PathNode(x, y, parent, cost, targetX, targetY);
        neighborList->push_front(newPathNode);
    };

    void MoveEndPosFiller::cleanUp(std::list<PathNode *> *nodes,
                                  std::unordered_map<int, PathNode*>* map,
                                  std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>* heap,
                                  std::unordered_map<int, PathNode*>* map2) {
        for (auto node : *nodes)
        {
            delete node;
        }
        delete nodes;

        delete map;
        delete map2;
        delete heap;
    }
}