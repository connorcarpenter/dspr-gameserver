//
// Created by connor on 7/23/18.
//

#include "Game.h"

namespace DsprGameServer {

    void Game::addPlayer(std::string token, uWS::WebSocket<1> *ws) {
        Player *p = new Player(token, ws);
        playerMap.insert(std::pair<std::string, Player *>(token, p));
        this->grid->sendGrid(p);
    }

    Game::Game() {
        grid = new TileManager(32, 32);
    }

    Game::~Game() {
        delete grid;

        for (const auto &playerPair : playerMap) {
            if (playerPair.second != nullptr)
                delete playerPair.second;
        }
    }

}