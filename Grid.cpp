//
// Created by connor on 7/23/18.
//

#include <cstdlib>
#include <sstream>
#include "Grid.h"

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
    this->tileArrayA = initializeTileArray(this->width, this->height);
    this->tileArrayB = initializeTileArray(this->width, this->height);
}

Grid::~Grid() {
    for (int j = 0; j < this->height; j += 1)
    {
        for (int i = 0;i<this->width;i+=1)
        {
            delete this->tileArrayA[(j*this->width)+i];
            delete this->tileArrayB[(j*this->width)+i];
        }
    }

    delete[] this->tileArrayA;
    delete[] this->tileArrayB;
}

int Grid::getRandom(int max){
    return std::rand()/((RAND_MAX + 1u)/max);
}

void Grid::sendGrid(Player* player) {
    for (int j = 0; j < this->height; j += 1)
    {
        for (int i = 0;i<this->width;i+=1)
        {
            Tile* tile = this->tileArrayA[(j*this->width)+i];
            std::stringstream msg1;
            msg1 << "tile/1.0/give|" << (i*2) << "," << (j*2) << "," << tile->frame << "\r\n";
            player->getWs()->send(msg1.str().c_str(), uWS::BINARY);

            tile = this->tileArrayB[(j*this->width)+i];
            std::stringstream msg2;
            msg2 << "tile/1.0/give|" << (i*2) << "," << (j*2) << "," << tile->frame << "\r\n";
            player->getWs()->send(msg2.str().c_str(), uWS::BINARY);
        }
    }
}

Tile **Grid::initializeTileArray(int width, int height) {
    Tile** output = new Tile*[width * height];
    for (int j = 0; j < height; j += 1)
    {
        for (int i = 0;i< width; i+=1)
        {
            int randomNumber = getRandom(100);
            int frameIndex = 5;
            if (randomNumber < 97) frameIndex = 4;
            if (randomNumber < 94) frameIndex = 3;
            if (randomNumber < 91) frameIndex = 8;
            if (randomNumber < 86) frameIndex = 7;
            if (randomNumber < 81) frameIndex = 6;
            if (randomNumber < 76) frameIndex = 2;
            if (randomNumber < 66) frameIndex = 1;
            if (randomNumber < 56) frameIndex = 0;

            output[(j*width)+i] = new Tile(frameIndex);
        }
    }
    return output;
}


