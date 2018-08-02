//
// Created by connor on 7/23/18.
//

#include <sstream>
#include <iostream>
#include "TileManager.h"
#include "GameServer.h"
#include "MathUtils.h"

namespace DsprGameServer {

    TileManager::TileManager(int width, int height) {
        this->width = width;
        this->height = height;
        this->tileArrayA = initializeTileArray(this->width, this->height);
        this->tileArrayB = initializeTileArray(this->width, this->height);
    }

    TileManager::~TileManager() {
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

        for (int j = 0; j < this->height; j += 1) {
            for (int i = 0; i < this->width; i += 1) {
                Tile *tile = this->tileArrayA[(j * this->width) + i];
                std::stringstream msg1;
                msg1 << "tile/1.0/create|" << (i * 2) << "," << (j * 2) << "," << tile->frame << "\r\n";
                GameServer::get().queueMessage(player->getWs(), msg1.str());
            }

            for (int i = 0; i < this->width; i += 1) {
                Tile *tile = this->tileArrayB[(j * this->width) + i];
                std::stringstream msg2;
                msg2 << "tile/1.0/create|" << ((i * 2) + 1) << "," << ((j * 2) + 1) << "," << tile->frame << "\r\n";
                GameServer::get().queueMessage(player->getWs(), msg2.str());
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
}