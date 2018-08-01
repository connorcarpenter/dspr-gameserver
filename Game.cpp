//
// Created by connor on 7/23/18.
//

#include "Game.h"

namespace DsprGameServer
{
    Game::Game()
    {
        tileManager = new TileManager(32, 32);
        unitManager = new UnitManager();
    }

    Game::~Game()
    {
        delete tileManager;
        delete unitManager;

        for (const auto &playerPair : playerMap) {
            if (playerPair.second != nullptr)
                delete playerPair.second;
        }
    }

    void Game::addPlayer(std::string token, uWS::WebSocket<1> *ws)
    {
        Player* p = new Player(token, ws);
        playerMap.insert(std::pair<std::string, Player *>(token, p));
        this->tileManager->sendGrid(p);
        this->unitManager->sendUnits(p);
    }
}