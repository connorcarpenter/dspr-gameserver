//
// Created by connor on 8/4/18.
//

#include <unordered_map>
#include <bits/unordered_map.h>
#include <set>
#include <queue>
#include "AStarPathfinder.h"
#include "PathTile.h"
#include "PathNode.h"
#include "../Game/UnitManager.h"

namespace DsprGameServer
{
    AStarPathfinder::AStarPathfinder(Game *game) {
        this->game = game;
    }

    std::shared_ptr<Path>
    AStarPathfinder::findPath(const std::list<std::pair<int, int>> &unitPositions, int targetX, int targetY,
                                  bool attacking)
    {
        auto targetTile = this->game->tileManager->getTileAt(targetX, targetY);
        if (targetTile == nullptr || !targetTile->walkable) return nullptr;

        auto path = std::make_shared<Path>(this->game, targetX, targetY);

        for (auto startPosition : unitPositions)
        {
            addToPath(path, startPosition.first, startPosition.second, targetX, targetY);
        }

        if (attacking){
            this->fillEndTilesMove(path, unitPositions.size() + 1);
        } else {
            this->fillEndTilesMove(path, unitPositions.size());
        }

        return path;
    }

    void AStarPathfinder::addToPath(std::shared_ptr<Path> path,
                                     int startX, int startY,
                                     int targetX, int targetY)
    {
        auto closedMap = new std::unordered_map<int, PathNode*>();
        auto openHeap = new std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>();
        auto openMap = new std::unordered_map<int, PathNode*>();
        auto nodes = new std::list<PathNode*>();

        //put first node into openHeap/Map
        auto startNode = new PathNode(startX, startY, nullptr, 0, targetX, targetY);
        openHeap->push(startNode);
        openMap->emplace(startNode->getId(), startNode);
        nodes->push_back(startNode);

        while(openMap->size() > 0)
        {
            PathNode* currentNode = openHeap->top(); /*and then delete it*/ openHeap->pop();
            openMap->erase(currentNode->getId());
            if (!path->foundEnd)
            {
                if (currentNode->x == targetX && currentNode->y == targetY)
                {
                    ///solution is found! now just create the path structure
                    path->foundEnd = true;

                    //create end tile
                    auto lastTile = new PathTile(currentNode->x, currentNode->y, 0);
                    path->addTile(lastTile);

                    //work backwards from current tile to find path
                    while (currentNode->parent != nullptr) {
                        auto prevNode = currentNode->parent;
                        auto prevTile = new PathTile(prevNode->x, prevNode->y, lastTile->disToEnd+disToEndUnits);
                        prevTile->nextTile = lastTile;
                        path->addTile(prevTile);
                        currentNode = prevNode;
                        lastTile = prevTile;
                    }

                    cleanUp(nodes, closedMap, openHeap, openMap);
                    return;
                }
            }
            else
            {
                auto tileInPath = path->getTile(currentNode->x, currentNode->y);
                if (tileInPath != nullptr)
                {
                    ///we found the path! now just add to the existing path
                    auto lastTile = tileInPath;

                    //work backwards from current tile to find path
                    while (currentNode->parent != nullptr) {
                        auto prevNode = currentNode->parent;
                        auto prevTile = new PathTile(prevNode->x, prevNode->y, lastTile->disToEnd+disToEndUnits);
                        prevTile->nextTile = lastTile;
                        path->addTile(prevTile);
                        currentNode = prevNode;
                        lastTile = prevTile;
                    }

                    cleanUp(nodes, closedMap, openHeap, openMap);
                    return;
                }
            }

            std::list<PathNode*>* neighbors = getNeighbors(currentNode, targetX, targetY, false);
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
                        //since g and h are re-evaluted, shouldn't we re-insert the new node back into the heap? so that it may bubble up to the top
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
        return;
    }

    std::list<PathNode *> * AStarPathfinder::getNeighbors(PathNode *parent, int targetX, int targetY, bool endFilling)
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

    void AStarPathfinder::tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj, int targetX,
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

    void AStarPathfinder::fillEndTilesMove(std::shared_ptr<Path> path, int unitNumber)
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
}
