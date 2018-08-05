//
// Created by connor on 8/4/18.
//

#include <unordered_map>
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
            PathTile* currentNode = findSmallestF(open_list);
            if (currentNode->x == endTile->x && currentNode->y == endTile->y)
            {
                //work backwards from current node to start
                auto path = std::make_shared<Path>(true);
                for (auto startPosition : unitPositions)
                {
                    auto newTile = new PathTile(startPosition.first, startPosition.second);
                    path->startTiles.emplace(newTile->getTileId(), newTile);
                }

                auto copiedTile = new PathTile(currentNode);
                path->endTiles.emplace(copiedTile->getTileId(), copiedTile);

                while(path->startTiles.count(currentNode->previousTileId) == 0)
                {
                    currentNode = closed_list->at(currentNode->previousTileId);
                    path->pathTiles.emplace(currentNode->getTileId(), currentNode);
                }

                return path;
            }
            open_list->erase(currentNode->getTileId());
            closed_list->emplace(currentNode->getTileId(), currentNode);

            std::unordered_map<int, PathTile*> neighbors = *getNeighbors(currentNode, targetX, targetY);
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

        auto newPathTile = new PathTile(parentTile->x + 2, parentTile->y, parentTile, 1.414f, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);
        newPathTile = new PathTile(parentTile->x - 2, parentTile->y, parentTile, 1.414f, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);
        newPathTile = new PathTile(parentTile->x, parentTile->y + 2, parentTile, 1.414f, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);
        newPathTile = new PathTile(parentTile->x, parentTile->y - 2, parentTile, 1.414f, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);

        newPathTile = new PathTile(parentTile->x + 1, parentTile->y + 1, parentTile, 1, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);
        newPathTile = new PathTile(parentTile->x - 1, parentTile->y - 1, parentTile, 1, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);
        newPathTile = new PathTile(parentTile->x + 1, parentTile->y - 1, parentTile, 1, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);
        newPathTile = new PathTile(parentTile->x - 1, parentTile->y + 1, parentTile, 1, targetX, targetY);
        neighbors->emplace(newPathTile->getTileId(), newPathTile);

        return neighbors;
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
