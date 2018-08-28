#pragma once

//
// Created by connor on 7/23/18.
//

#include "Tile.h"
#include "../Player.h"
#include "Game.h"

namespace DsprGameServer {
    class Game;

    class TileManager {
    public:
        TileManager(Game* game, int width, int height);

        ~TileManager();

        void sendGrid(Player *player);

        Tile* getTileAt(int x, int y);

        int width;
        int height;
    private:


        Tile **tileArrayA = nullptr;
        Tile **tileArrayB = nullptr;

        Tile **initializeTileArray(int width, int height);

        int getGridIndex(int x, int y);

        int getTileIndex(int gridIndex, int x, int y);

        void makeRandomWall();

        Game *game = nullptr;
    };
}