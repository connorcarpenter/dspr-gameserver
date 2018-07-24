#pragma once

//
// Created by connor on 7/23/18.
//

#include "Tile.h"
#include "Player.h"

class Grid {
public:
    Grid(int width, int height);
    ~Grid();

    void sendGrid(Player *player);

private:
    int width;
    int height;

    Tile** tileArrayA = nullptr;
    Tile** tileArrayB = nullptr;

    int getRandom(int max);

    Tile **initializeTileArray(int width, int height);


};