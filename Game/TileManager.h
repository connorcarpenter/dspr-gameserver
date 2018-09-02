#pragma once

//
// Created by connor on 7/23/18.
//

#include "Tile.h"
#include "../Player.h"
#include "Game.h"
#include "../IsoGrid.h"

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

        void makeRandomWall();

        Game *game = nullptr;

        IsoGrid<Tile*>* tileGrid = nullptr;

        void initializeTiles();
    };
}