//
// Created by connor on 8/31/18.
//

#include <unordered_map>
#include <queue>
#include "SimplePathfinder.h"
#include "PathNode.h"
#include "../Game/Unit.h"
#include "Path.h"
#include "PathTile.h"
#include "../Game/TileManager.h"
#include "../Game/UnitManager.h"
#include "../Game/OrderGroup.h"
#include "../Math/MathUtils.h"

namespace DsprGameServer {

    SimplePathfinder::SimplePathfinder(DsprGameServer::Game *game) {
        this->game = game;
    }

    Point *SimplePathfinder::findNextPosition(Unit *unit, std::shared_ptr<DsprGameServer::Path> path) {
        auto closedMap = new std::unordered_map<int, PathNode*>();
        auto openHeap = new std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>();
        auto openMap = new std::unordered_map<int, PathNode*>();
        auto nodes = new std::list<PathNode*>();

        //find targetX, targetY, from path, unit's lastKnownTile
        int targetX = 0;
        int targetY = 0;

        if (unit->lastKnownLongPathTile == nullptr){
            targetX = path->targetX; targetY = path->targetY;
        } else {
            auto targetTile = unit->lastKnownLongPathTile;
            int scanLength = this->maxNodeCost/this->straightCost;
            while (scanLength > 0 && targetTile->nextTile != nullptr) {
                targetTile = targetTile->nextTile;
                scanLength -= 1;
            }
            targetX = targetTile->x;
            targetY = targetTile->y;
        }

        //put first node into openHeap/Map
        auto startNode = new PathNode(unit->position->x, unit->position->y, nullptr, 0, targetX, targetY);
        int maxDisToConsider = startNode->h;
        openHeap->push(startNode);
        openMap->emplace(startNode->getId(), startNode);
        nodes->push_back(startNode);

        while(openMap->size() > 0) {
            PathNode *currentNode = openHeap->top(); /*and then delete it*/ openHeap->pop();
            openMap->erase(currentNode->getId());
            closedMap->emplace(currentNode->getId(), currentNode);

            //check if we've hit the path yet
            auto tileInPath = path->getTile(currentNode->x, currentNode->y);
            bool pathFindSuccess = (tileInPath != nullptr && tileInPath->disToEnd < unit->disToEnd);
            bool withinEndDis = false;
            if (!pathFindSuccess) {
                withinEndDis = MathUtils::Distance(currentNode->x, currentNode->y, path->targetX, path->targetY) <=
                               unit->orderGroup->getAcceptableTilesToEnd();
                if (withinEndDis) pathFindSuccess = true;
            }

            if (pathFindSuccess)
            {
                int newDisToEnd = (tileInPath != nullptr) ? tileInPath->disToEnd : (int) withinEndDis * 5;
                if (tileInPath != nullptr){
                    unit->lastKnownLongPathTile = tileInPath;
                }
                //path tile is closer to end, this is our guy!

                unit->shortPath = std::make_shared<Path>(currentNode->x, currentNode->y);

                PathTile* prevTile = new PathTile(currentNode->x, currentNode->y, newDisToEnd);
                unit->shortPath->addTile(prevTile);

                int disToEndCount = newDisToEnd;

                //work backwards from current tile till we're 1 step away from start
                while (currentNode->parent != startNode && currentNode->parent != nullptr) {
                    currentNode = currentNode->parent;
                    disToEndCount += 10;

                    PathTile *newTile = new PathTile(currentNode->x, currentNode->y, disToEndCount);
                    newTile->nextTile = prevTile;
                    unit->shortPath->addTile(newTile);
                    prevTile = newTile;
                }

                unit->shortPathCurrentTile = prevTile;
                unit->disToEnd = prevTile->disToEnd;

                int newX = currentNode->x;
                int newY = currentNode->y;
                cleanUp(nodes, closedMap, openHeap, openMap);

                return new Point(newX, newY);
            }

            //expand nodes
            if (currentNode->g < this->maxNodeCost)
            {
                std::list<PathNode *> *neighbors = getNeighbors(currentNode, targetX, targetY, unit);
                for (auto newNode : *neighbors) {
                    if (newNode->h > maxDisToConsider + unit->lostWithoutShortPath){
                        delete newNode;
                        continue;
                    }

                    if (closedMap->count(newNode->getId()) != 0) {
                        delete newNode;
                        continue;
                    }

                    if (openMap->count(newNode->getId()) == 0) {
                        openHeap->push(newNode);
                        openMap->emplace(newNode->getId(), newNode);
                        nodes->push_back(newNode);
                    } else {
                        auto existingNode = openMap->at(newNode->getId());

                        if (newNode->g < existingNode->g) {
                            existingNode->g = newNode->g;
                            existingNode->parent = newNode->parent;
                            //since g is re-evaluted, shouldn't we re-insert the new node back into the heap? so that it may bubble up to the top?
                        }

                        delete newNode;
                        continue;
                    }
                }

                //clean up neighbor list
                delete neighbors;
            }
        }

        //couldn't find nothin
        cleanUp(nodes, closedMap, openHeap, openMap);
        return nullptr;
    }

    std::list<PathNode *> * SimplePathfinder::getNeighbors(PathNode *parent, int targetX, int targetY, Unit *unit)
    {
        auto neighbors = new std::list<PathNode*>();

        if (parent->g + this->diagonalCost <= this->maxNodeCost)
        {
            tryAddNeighbor(neighbors, parent, 2, 0, this->diagonalCost, targetX, targetY, unit);
            tryAddNeighbor(neighbors, parent, -2, 0, this->diagonalCost, targetX, targetY, unit);
            tryAddNeighbor(neighbors, parent, 0, 2, this->diagonalCost, targetX, targetY, unit);
            tryAddNeighbor(neighbors, parent, 0, -2, this->diagonalCost, targetX, targetY, unit);
        }

        if (parent->g + this->straightCost <= this->maxNodeCost)
        {
            tryAddNeighbor(neighbors, parent, 1, 1, this->straightCost, targetX, targetY, unit);
            tryAddNeighbor(neighbors, parent, -1, -1, this->straightCost, targetX, targetY, unit);
            tryAddNeighbor(neighbors, parent, 1, -1, this->straightCost, targetX, targetY, unit);
            tryAddNeighbor(neighbors, parent, -1, 1, this->straightCost, targetX, targetY, unit);
        }

        return neighbors;
    }

    void
    SimplePathfinder::tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj, float cost,
                                         int targetX, int targetY, Unit *unit)
    {

        int x = parent->x + xAdj;
        int y = parent->y + yAdj;
        auto tile = this->game->tileManager->getTileAt(x, y);
        if (tile == nullptr) return;
        if (!tile->walkable) return;
        auto otherUnit = this->game->unitManager->getUnitFromGrid(x, y);
        if (otherUnit != nullptr && (!unit->shouldPushOtherUnit(otherUnit, true))) return;

        auto newPathNode = new PathNode(x, y, parent, cost, targetX, targetY);
        neighborList->push_front(newPathNode);
    };

    void SimplePathfinder::cleanUp(std::list<PathNode *> *nodes,
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