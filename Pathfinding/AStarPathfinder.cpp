//
// Created by connor on 8/4/18.
//

#include <unordered_map>
#include <bits/unordered_map.h>
#include <set>
#include <queue>
#include "AStarPathfinder.h"
#include "PathTile.h"

namespace DsprGameServer
{
    AStarPathfinder::AStarPathfinder(DsprGameServer::TileManager *tileManager) {
        this->tileManager = tileManager;
    }

    std::shared_ptr<Path> AStarPathfinder::findPath(const std::list<std::pair<int,int>>& unitPositions,
                                     int unitNumber,
                                     int targetX,
                                     int targetY)
    {
        auto closedMap = new std::unordered_map<int, PathNode*>();
        auto openHeap = new std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>();
        auto openMap = new std::unordered_map<int, PathNode*>();
        auto nodes = new std::list<PathNode*>();

        for (auto startPosition : unitPositions)
        {
            auto newNode = new PathNode(startPosition.first, startPosition.second, nullptr, 0, targetX, targetY);
            openHeap->push(newNode);
            openMap->emplace(newNode->getId(), newNode);
            nodes->push_back(newNode);
        }

        while(openMap->size() > 0)
        {
            PathNode* currentNode = openHeap->top(); /*and then delete it*/ openHeap->pop();
            openMap->erase(currentNode->getId());
            if (currentNode->x == targetX && currentNode->y == targetY)
            {
                ///solution is found! now just create the path structure
                auto path = std::make_shared<Path>(true);

                //create end tile
                auto lastTile = new PathTile(currentNode->x, currentNode->y);
                path->addEndTile(lastTile);
                path->addTargetTile(lastTile);

                //work backwards from current tile to find path
                while(currentNode->parent != nullptr)
                {
                    auto prevNode = currentNode->parent;
                    auto prevTile = new PathTile(prevNode->x, prevNode->y);
                    prevTile->nextTile = lastTile;

                    if (prevNode->parent == nullptr)
                    {
                        //prevNode is now a starting tile
                        path->addStartTile(prevTile);
                    }
                    else
                    {
                        path->addPathTile(prevTile);
                    }

                    currentNode = prevNode;
                    lastTile = prevTile;
                }

                cleanUp(nodes, closedMap, openHeap, openMap);

                return path;
            }

            std::list<PathNode*>* neighbors = getNeighbors(currentNode, targetX, targetY);
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
                    auto existingNode = openMap->at(newNode->getId());

                    if (newNode->g < existingNode->g)
                    {
                        existingNode->g = newNode->g;
                        existingNode->h = newNode->h;
                        existingNode->parent = newNode->parent;
                    }

                    delete newNode;
                    continue;
                }
            }

            //clean up neighbor list
            delete neighbors;

            closedMap->emplace(currentNode->getId(), currentNode);
        }

        cleanUp(nodes, closedMap, openHeap, openMap);

        return std::make_shared<Path>(false);
    }

    std::list<PathNode*>* AStarPathfinder::getNeighbors(PathNode* parent, int targetX, int targetY)
    {
        auto neighbors = new std::list<PathNode*>();

        tryAddNeighbor(neighbors, parent, 2,  0, targetX, targetY, this->diagonalCost);
        tryAddNeighbor(neighbors, parent,-2,  0, targetX, targetY, this->diagonalCost);
        tryAddNeighbor(neighbors, parent, 0,  2, targetX, targetY, this->diagonalCost);
        tryAddNeighbor(neighbors, parent, 0, -2, targetX, targetY, this->diagonalCost);

        tryAddNeighbor(neighbors, parent, 1,  1, targetX, targetY, this->straightCost);
        tryAddNeighbor(neighbors, parent,-1, -1, targetX, targetY, this->straightCost);
        tryAddNeighbor(neighbors, parent, 1, -1, targetX, targetY, this->straightCost);
        tryAddNeighbor(neighbors, parent,-1,  1, targetX, targetY, this->straightCost);

        return neighbors;
    }

    void AStarPathfinder::tryAddNeighbor(std::list<PathNode*>* neighborList, PathNode* parent,
                                             int xAdj, int yAdj, int targetX, int targetY, float cost)
    {

        int x = parent->x + xAdj;
        int y = parent->y + yAdj;
        auto tile = tileManager->getTileAt(x, y);
        if (tile == nullptr) return;
        if (!tile->walkable) return;

        auto newPathNode = new PathNode(x, y, parent, cost, targetX, targetY);
        neighborList->push_front(newPathNode);
    };

    int AStarPathfinder::mapWidth = 0;

    void AStarPathfinder::setMapWidth(int width) {
        AStarPathfinder::mapWidth = width;
    }

    int AStarPathfinder::getMapWidth() {
        return AStarPathfinder::mapWidth;
    }

    void AStarPathfinder::cleanUp(std::list<PathNode *> *nodes,
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
