//
// Created by connor on 7/23/18.
//

#include "Game.h"

void Game::addPlayer(std::string token, uWS::WebSocket<1> *ws)
{
    Player* p = new Player(token, ws);
    playerMap.insert(p);
    this->grid->sendGrid(p);
}

Game::Game() {
    grid = new Grid(32, 32);
}

Game::~Game() {
    delete grid;

    for(const auto& playerPair : playerMap)
    {
        if (playerPair.second != nullptr)
            delete playerPair.second;
    }
}
