//
// Created by connor on 7/23/18.
//

#include <sstream>
#include <iostream>
#include "TileManager.h"
#include "../GameServer.h"
#include "../Math/MathUtils.h"

namespace DsprGameServer
{
    TileManager::TileManager(Game *game, int width, int height)
    {
        this->game = game;
        this->width = width;
        this->height = height;
        this->tileArrayA = initializeTileArray(this->width, this->height);
        this->tileArrayB = initializeTileArray(this->width, this->height);
        for(int i=0;i<40;i+=1)
            this->makeRandomWall();
    }

    TileManager::~TileManager()
    {
        for (int j = 0; j < this->height; j += 1) {
            for (int i = 0; i < this->width; i += 1) {
                delete this->tileArrayA[(j * this->width) + i];
                delete this->tileArrayB[(j * this->width) + i];
            }
        }

        delete[] this->tileArrayA;
        delete[] this->tileArrayB;
    }

    void TileManager::sendGrid(Player* player)
    {
        std::stringstream msg0;
        msg0 << "grid/1.0/create|" << this->width << "," << this->height << "\r\n";
        GameServer::get().queueMessage(player->getWs(), msg0.str());

        for (int j = 0; j < this->height; j += 1)
        {
            for (int i = 0; i < this->width; i += 1)
            {
                Tile *tile = this->tileArrayA[(j * this->width) + i];
                if (tile->walkable)
                {
                    std::stringstream msg;
                    msg << "tile/1.0/create|" << (i * 2) << "," << (j * 2) << "," << tile->frame << "\r\n";
                    GameServer::get().queueMessage(player->getWs(), msg.str());
                }
                else
                {
                    std::stringstream msg;
                    msg << "tile/1.0/create|" << (i * 2) << "," << (j * 2) << ",-1" << "\r\n";
                    GameServer::get().queueMessage(player->getWs(), msg.str());
                }
            }

            for (int i = 0; i < this->width; i += 1)
            {
                Tile *tile = this->tileArrayB[(j * this->width) + i];
                if (tile->walkable)
                {
                    std::stringstream msg;
                    msg << "tile/1.0/create|" << ((i * 2) + 1) << "," << ((j * 2) + 1) << "," << tile->frame << "\r\n";
                    GameServer::get().queueMessage(player->getWs(), msg.str());
                }
                else
                {
                    std::stringstream msg;
                    msg << "tile/1.0/create|" << ((i * 2) + 1) << "," << ((j * 2) + 1) << ",-1" << "\r\n";
                    GameServer::get().queueMessage(player->getWs(), msg.str());
                }
            }
        }
    }

    Tile **TileManager::initializeTileArray(int width, int height)
    {
        Tile **output = new Tile *[width * height];
        for (int j = 0; j < height; j += 1) {
            for (int i = 0; i < width; i += 1) {
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

                output[(j * width) + i] = new Tile(frameIndex);
            }
        }
        return output;
    }

    Tile* TileManager::getTileAt(int x, int y)
    {
        int gridIndex = getGridIndex(x, y);
        if (gridIndex < 0) return nullptr;

        int tileIndex = getTileIndex(gridIndex, x, y);
        if (gridIndex == 0)
        {
            return this->tileArrayA[tileIndex];
        }
        else
        {
            return this->tileArrayB[tileIndex];
        }
    }

    int TileManager::getGridIndex(int x, int y)
    {
        if (x < 0 || y < 0 || x >= this->width*2 || y >= this->height*2) return -1;

        if (x % 2 == 0 && y % 2 == 0) return 0;
        if ((x+1) % 2 == 0 && (y+1) % 2 == 0) return 1;
        return -2;
    }

    int TileManager::getTileIndex(int gridIndex, int x, int y)
    {
        if (gridIndex == 0)
        {
            int xsmall = x / 2;
            int ysmall = y / 2;
            return (ysmall * this->width) + xsmall;
        }
        else
        {
            int xsmall = (x-1) / 2;
            int ysmall = (y-1) / 2;
            return (ysmall * this->height) + xsmall;
        }
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
}