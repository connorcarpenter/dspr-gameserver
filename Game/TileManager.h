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

        Tile* getTileAt(int x, int y);
        void sendGrid(PlayerData *playerData);
        void sendTileToPlayer(int x, int y, PlayerData *playerData);

        int width;
        int height;

    private:

        void makeRandomWall();
        void initializeTiles();
        void sendAllDiscoveredTilesToPlayer(PlayerData *playerData);

        Game *game = nullptr;
        PtrIsoGrid<Tile*>* tileGrid = nullptr;
    };
}