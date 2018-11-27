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
#include "../DsprMessage/ToClientMsg.h"
#include "PlaneGeneration/PlaneGenerator.h"

namespace DsprGameServer
{
    TileManager::TileManager(Game *game, int width, int height)
    {
        this->game = game;
        this->width = width;
        this->height = height;

        this->tileGrid = new PtrIsoGrid<Tile*>();
        this->tileGrid->initialize(this->width * 2, this->height * 2);
        //this->initializeTiles();

        this->game->planeGenerator->fill(this->tileGrid);
    }

    TileManager::~TileManager()
    {
        delete this->tileGrid;
    }

    void TileManager::sendGrid(PlayerData *playerData)
    {
        DsprMessage::GridCreateMsgV1 gridCreateMsgV1;
        gridCreateMsgV1.width.set(this->width);
        gridCreateMsgV1.height.set(this->height);
        auto clientMsg = gridCreateMsgV1.getToClientMessage();
        auto packedMsg = clientMsg->Pack();
        GameServer::get().queueMessage(playerData, packedMsg);

        this->sendAllDiscoveredTilesToPlayer(playerData);
    }

    void TileManager::initializeTiles()
    {
        this->tileGrid->forEachCoord([&](int x, int y){
            int randomNumber = MathUtils::getRandom(100);
            int frameIndex = 5;
            if (randomNumber < 99) frameIndex = 4;
            if (randomNumber < 98) frameIndex = 3;
            if (randomNumber < 97) frameIndex = 2;
            if (randomNumber < 95) frameIndex = 1;
            if (randomNumber < 93) frameIndex = 0;
            if (randomNumber < 91) frameIndex = 8;
            if (randomNumber < 70) frameIndex = 7;
            if (randomNumber < 50) frameIndex = 6;

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
        int frameToSend = (tile->walkable) ? (tile->frame+1) : 0;

        DsprMessage::TileCreateMsgV1 tileCreateMsgV1;
        tileCreateMsgV1.x.set(x);
        tileCreateMsgV1.y.set(y);
        tileCreateMsgV1.frame.set(frameToSend);
        auto clientMsg = tileCreateMsgV1.getToClientMessage();
        auto packedMsg = clientMsg->Pack();
        GameServer::get().queueMessage(playerData, packedMsg);

    }

    void TileManager::sendAllDiscoveredTilesToPlayer(PlayerData *playerData) {
        this->game->fogManager->forEachFogTile(playerData->getTribe(),
            [&](int fogAmount, int x, int y)
            {
                if (fogAmount == 0) return;

                auto tile = this->tileGrid->get(x,y);
                if (tile == nullptr) return;
                int frameToSend = (tile->walkable) ? (tile->frame+1) : 0;

                DsprMessage::TileCreateMsgV1 tileCreateMsgV1;
                tileCreateMsgV1.x.set(x);
                tileCreateMsgV1.y.set(y);
                tileCreateMsgV1.frame.set(frameToSend);
                auto clientMsg = tileCreateMsgV1.getToClientMessage();
                auto packedMsg = clientMsg->Pack();
                GameServer::get().queueMessage(playerData, packedMsg);
            });
    }
}