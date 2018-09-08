//
// Created by connor on 7/23/18.
//

#include <sstream>
#include <iostream>
#include "TileManager.h"
#include "../GameServer.h"
#include "../Math/MathUtils.h"
#include "FogManager.h"
#include "../PlayerData.h"

namespace DsprGameServer
{
    TileManager::TileManager(Game *game, int width, int height)
    {
        this->game = game;
        this->width = width;
        this->height = height;

        this->tileGrid = new PtrIsoGrid<Tile*>();
        this->tileGrid->initialize(this->width * 2, this->height * 2);
        this->initializeTiles();

        for(int i=0;i<40;i+=1)
            this->makeRandomWall();
    }

    TileManager::~TileManager()
    {
        delete this->tileGrid;
    }

    void TileManager::sendGrid(PlayerData *playerData)
    {
        std::stringstream msg0;
        msg0 << "grid/1.0/create|" << this->width << "," << this->height << "\r\n";
        GameServer::get().queueMessage(playerData, msg0.str());

        this->sendAllDiscoveredTilesToPlayer(playerData);
    }

    void TileManager::initializeTiles()
    {
        this->tileGrid->forEachCoord([&](int x, int y){
            int randomNumber = MathUtils::getRandom(100);
            int frameIndex = 5;
            if (randomNumber < 99) frameIndex = 4;
            if (randomNumber < 98) frameIndex = 3;
            if (randomNumber < 97) frameIndex = 8;
            if (randomNumber < 95) frameIndex = 7;
            if (randomNumber < 93) frameIndex = 6;
            if (randomNumber < 91) frameIndex = 2;
            if (randomNumber < 70) frameIndex = 1;
            if (randomNumber < 50) frameIndex = 0;

            this->tileGrid->set(x, y, new Tile(frameIndex));
        });
    }

    Tile* TileManager::getTileAt(int x, int y)
    {
        return this->tileGrid->get(x,y);
    }

    void TileManager::makeRandomWall()
    {
        int startX = MathUtils::getRandom(this->width)*2;
        int startY = MathUtils::getRandom(this->height)*2;
        int gridIndex = MathUtils::getRandom(2);
        if (gridIndex != 0) { startX += 1; startY += 1; };
        int dirX = MathUtils::getRandom(3)-1;
        int dirY = MathUtils::getRandom(3)-1;
        if (dirX == 0 && dirY == 0) dirX = 1;
        if (dirX == 0 || dirY == 0) { dirX*=2; dirY*=2; }
        int length = MathUtils::getRandom(12)+3;

        for(int i=0;i<length;i+=1){
            int wallX = startX + i*dirX;
            int wallY = startY + i*dirY;
            auto wallTile = getTileAt(wallX, wallY);
            if (wallTile == nullptr) break;
            wallTile->walkable = false;
        }

        if (dirX == 2 || dirY == 2 || dirX == -2 || dirY == -2)
        {
            for(int i=0;i<length;i+=1){
                int wallX = 1 + startX + i*dirX;
                int wallY = 1 + startY + i*dirY;
                auto wallTile = getTileAt(wallX, wallY);
                if (wallTile == nullptr) break;
                wallTile->walkable = false;
            }
        }
    }

    void TileManager::sendTileToPlayer(int x, int y, PlayerData *playerData) {
        if (playerData == nullptr) return;

        auto tile = this->tileGrid->get(x,y);
        if (tile->walkable)
        {
            std::stringstream msg;
            msg << "tile/1.0/create|" << (x) << "," << (y) << "," << tile->frame << "\r\n";
            GameServer::get().queueMessage(playerData, msg.str());
        }
        else
        {
            std::stringstream msg;
            msg << "tile/1.0/create|" << (x) << "," << (y) << ",-1" << "\r\n";
            GameServer::get().queueMessage(playerData, msg.str());
        }
    }

    void TileManager::sendAllDiscoveredTilesToPlayer(PlayerData *playerData) {
        this->game->fogManager->forEachFogTile(playerData->getTribe(),
            [&](int fogAmount, int x, int y)
            {
                if (fogAmount == 0) return;

                auto tile = this->tileGrid->get(x,y);
                if (tile->walkable)
                {
                    std::stringstream msg;
                    msg << "tile/1.0/create|" << (x) << "," << (y) << "," << tile->frame << "\r\n";
                    GameServer::get().queueMessage(playerData, msg.str());
                }
                else
                {
                    std::stringstream msg;
                    msg << "tile/1.0/create|" << (x) << "," << (y) << ",-1" << "\r\n";
                    GameServer::get().queueMessage(playerData, msg.str());
                }
            });
    }
}