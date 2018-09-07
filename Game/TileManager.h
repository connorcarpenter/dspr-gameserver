#pragma once

//
// Created by connor on 7/23/18.
//

#include "Tile.h"
#include "Game.h"
#include "../PtrIsoGrid.h"

namespace DsprGameServer {

    class Game;

    class TileManager {
    public:
        TileManager(Game* game, int width, int height);

        ~TileManager();

        void sendGrid(PlayerData *playerData);

        Tile* getTileAt(int x, int y);

        int width;
        int height;
    private:

        void makeRandomWall();

        Game *game = nullptr;

        PtrIsoGrid<Tile*>* tileGrid = nullptr;

        void initializeTiles();
    };
}