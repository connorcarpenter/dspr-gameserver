//
// Created by connor on 8/4/18.
//

#include <unordered_map>
#include <bits/unordered_map.h>
#include "AStarPathfinder.h"

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
        PathTile* endTile = new PathTile(targetX, targetY);

        //pathfinding
        auto closed_list = new std::unordered_map<int, PathTile*>();
        auto open_list = new std::unordered_map<int, PathTile*>();

        for (auto startPosition : unitPositions)
        {
            auto newTile = new PathTile(startPosition.first, startPosition.second, targetX, targetY);
            open_list->emplace(newTile->getTileId(), newTile);
        }

        while(!open_list->empty())
        {
            PathTile* currentTile = findSmallestF(open_list);
            if (currentTile->x == endTile->x && currentTile->y == endTile->y)
            {
                //work backwards from current node to start
                auto path = std::make_shared<Path>(true);
                for (auto startPosition : unitPositions)
                {
                    auto newTile = new PathTile(startPosition.first, startPosition.second);
                    path->startTiles.emplace(newTile->getTileId(), newTile);
                }

                auto copiedTile = new PathTile(currentTile);
                path->endTiles.emplace(copiedTile->getTileId(), copiedTile);

                while(path->startTiles.count(currentTile->previousTileId) == 0)
                {
                    auto prevTile = closed_list->at(currentTile->previousTileId);
                    prevTile->setNextTile(currentTile->getTileId());
                    currentTile = prevTile;
                    path->pathTiles.emplace(currentTile->getTileId(), currentTile);
                }

                auto startTile = path->startTiles.at(currentTile->previousTileId);
                startTile->setNextTile(currentTile->getTileId());

                return path;
            }
            open_list->erase(currentTile->getTileId());
            closed_list->emplace(currentTile->getTileId(), currentTile);

            std::unordered_map<int, PathTile*> neighbors = *getNeighbors(currentTile, targetX, targetY);
            for(auto newTilePair : neighbors)
            {
                auto newTile = newTilePair.second;
                if (closed_list->count(newTile->getTileId()) != 0)
                    continue;
                if (open_list->count(newTile->getTileId()) == 0)
                {
                    open_list->emplace(newTile->getTileId(), newTile);
                }
                else
                {
                    auto findIt = open_list->find(newTile->getTileId());
                    PathTile* actualNode = findIt.operator*().second;
                    if (newTile->g < actualNode->g)
                    {
                        actualNode->g = newTile->g;
                        actualNode->h = newTile->h;
                        actualNode->setPreviousTile(newTile->previousTileId);
                    }
                }
            }
        }

        return std::make_shared<Path>(false);
    }

    std::unordered_map<int, PathTile*>* AStarPathfinder::getNeighbors(const PathTile *parentTile, int targetX, int targetY)
    {
        auto neighbors = new std::unordered_map<int, PathTile*>();

        tryAddNeighbor(neighbors, parentTile, 2,  0, targetX, targetY, 1.414f);
        tryAddNeighbor(neighbors, parentTile,-2,  0, targetX, targetY, 1.414f);
        tryAddNeighbor(neighbors, parentTile, 0,  2, targetX, targetY, 1.414f);
        tryAddNeighbor(neighbors, parentTile, 0, -2, targetX, targetY, 1.414f);

        tryAddNeighbor(neighbors, parentTile, 1,  1, targetX, targetY, 1.0f);
        tryAddNeighbor(neighbors, parentTile,-1, -1, targetX, targetY, 1.0f);
        tryAddNeighbor(neighbors, parentTile, 1, -1, targetX, targetY, 1.0f);
        tryAddNeighbor(neighbors, parentTile,-1,  1, targetX, targetY, 1.0f);

        return neighbors;
    }

    void AStarPathfinder::tryAddNeighbor(std::unordered_map<int, PathTile*>* neighborMap, const PathTile* parentTile,
                                             int tileX, int tileY, int targetX, int targetY, float cost)
    {
        int x = parentTile->x + tileX;
        int y = parentTile->y + tileY;
        auto tile = tileManager->getTileAt(x, y);
        if (tile == nullptr) return;
        if (!tile->walkable) return;

        auto newPathTile = new PathTile(x, y, parentTile, cost, targetX, targetY);
        neighborMap->emplace(newPathTile->getTileId(), newPathTile);
    };

    PathTile* AStarPathfinder::findSmallestF(std::unordered_map<int, PathTile*>* list)
    {
        float minTileVal = 99999;
        PathTile* result = nullptr;
        for(auto tile : *list)
        {
            if(tile.second->getF() < minTileVal)
            {
                minTileVal = tile.second->getF();
                result = tile.second;
            }
        }

        return result;
    }

    int AStarPathfinder::mapWidth = 0;

    void AStarPathfinder::setMapWidth(int width) {
        AStarPathfinder::mapWidth = width;
    }

    int AStarPathfinder::getMapWidth() {
        return AStarPathfinder::mapWidth;
    }
}
