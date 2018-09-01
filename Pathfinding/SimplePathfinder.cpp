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

        //put first node into openHeap/Map
        auto startNode = new PathNode(unit->position->x, unit->position->y, nullptr, 0, false);
        openHeap->push(startNode);
        openMap->emplace(startNode->getId(), startNode);
        nodes->push_back(startNode);

        while(openMap->size() > 0) {
            PathNode *currentNode = openHeap->top(); /*and then delete it*/ openHeap->pop();
            openMap->erase(currentNode->getId());
            closedMap->emplace(currentNode->getId(), currentNode);

            //check if we've hit the path yet
            auto tileInPath = path->getTile(currentNode->x, currentNode->y);
            bool withinEndDis = MathUtils::Distance(currentNode->x, currentNode->y, path->targetX, path->targetY) <= unit->orderGroup->getAcceptableTilesToEnd();
            bool pathFindSuccess = withinEndDis || (tileInPath != nullptr && tileInPath->disToEnd < unit->disToEnd);

            if (pathFindSuccess)
            {
                //path tile is closer to end, this is our guy!
                if (tileInPath != nullptr)
                {
                    unit->disToEnd = tileInPath->disToEnd;
                }
                else
                {
                    unit->disToEnd = (int) withinEndDis * 5;
                }

                unit->shortPath = std::make_shared<Path>(currentNode->x, currentNode->y);

                PathTile* prevTile = new PathTile(currentNode->x, currentNode->y, unit->disToEnd);
                unit->shortPath->addTile(prevTile);

                int disToEndCount = unit->disToEnd;

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

                int newX = currentNode->x;
                int newY = currentNode->y;
                cleanUp(nodes, closedMap, openHeap, openMap);

                return new Point(newX, newY);
            }

            //expand nodes
            if (currentNode->g < this->maxNodeCost)
            {
                std::list<PathNode *> *neighbors = getNeighbors(currentNode, unit);
                for (auto newNode : *neighbors) {
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

    std::list<PathNode *> * SimplePathfinder::getNeighbors(PathNode *parent, Unit *unit)
    {
        auto neighbors = new std::list<PathNode*>();

        if (parent->g + this->diagonalCost <= this->maxNodeCost)
        {
            tryAddNeighbor(neighbors, parent, 2, 0, this->diagonalCost, unit);
            tryAddNeighbor(neighbors, parent, -2, 0, this->diagonalCost, unit);
            tryAddNeighbor(neighbors, parent, 0, 2, this->diagonalCost, unit);
            tryAddNeighbor(neighbors, parent, 0, -2, this->diagonalCost, unit);
        }

        if (parent->g + this->straightCost <= this->maxNodeCost)
        {
            tryAddNeighbor(neighbors, parent, 1, 1, this->straightCost, unit);
            tryAddNeighbor(neighbors, parent, -1, -1, this->straightCost, unit);
            tryAddNeighbor(neighbors, parent, 1, -1, this->straightCost, unit);
            tryAddNeighbor(neighbors, parent, -1, 1, this->straightCost, unit);
        }

        return neighbors;
    }

    void
    SimplePathfinder::tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj, float cost,
                                         Unit *unit)
    {

        int x = parent->x + xAdj;
        int y = parent->y + yAdj;
        auto tile = this->game->tileManager->getTileAt(x, y);
        if (tile == nullptr) return;
        if (!tile->walkable) return;
        auto otherUnit = this->game->unitManager->getUnitWithNextPosition(x, y);
        if (otherUnit != nullptr) return;

        auto newPathNode = new PathNode(x, y, parent, cost, false);
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