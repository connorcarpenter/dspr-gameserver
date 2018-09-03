//
// Created by connor on 8/4/18.
//

#include "Path.h"
#include "PathTile.h"
#include "../Game/UnitManager.h"

namespace DsprGameServer
{
    Path::Path(Game *game, int targetX, int targetY)
    {
        this->game = game;
        this->targetX = targetX;
        this->targetY = targetY;
    }

    Path::~Path()
    {
        for(auto tile : tiles)
            delete tile.second;

        for(auto endTilePair : endTiles) {
            auto endTile = endTilePair.second;
            this->game->unitManager->removeEndPosAtCoord(endTile->x, endTile->y);
            delete endTilePair.second;
        }
    }

    PathTile* Path::getTile(int x, int y)
    {
        int tileId = PathTile::getTileId(x,y);
        if (tiles.count(tileId) == 0) return nullptr;
        return tiles.at(tileId);
    }

    void Path::addTile(PathTile *tile)
    {
        tiles.emplace(tile->getTileId(), tile);
    }

    PathTile* Path::getEndTile(int x, int y)
    {
        int tileId = PathTile::getTileId(x,y);
        if (endTiles.count(tileId) == 0) return nullptr;
        return endTiles.at(tileId);
    }

    void Path::addEndTile(PathTile *tile)
    {
        this->game->unitManager->addEndPosAtCoord(tile->x, tile->y);
        endTiles.emplace(tile->getTileId(), tile);
    }
}