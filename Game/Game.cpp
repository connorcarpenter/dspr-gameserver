//
// Created by connor on 7/23/18.
//

#include "Game.h"
#include "TileManager.h";
#include "UnitManager.h";
#include "TribeManager.h";
#include "../Pathfinding/AStarPathfinder.h"
#include "../Pathfinding/SimplePathfinder.h"

namespace DsprGameServer
{
    Game::Game()
    {
        this->tileManager = new TileManager(this, 64, 64);
        AStarPathfinder::setMapWidth(64);
        this->tribeManager = new TribeManager(this);
        this->unitManager = new UnitManager(this);
        this->unitManager->initializeFirstUnits();
        this->pathfinder = new AStarPathfinder(this);
        this->simplePathfinder = new SimplePathfinder(this);
    }

    Game::~Game()
    {
        delete this->tileManager;
        delete this->unitManager;
        delete this->pathfinder;
        delete this->tribeManager;

        for (const auto &playerPair : playerMap) {
            if (playerPair.second != nullptr)
                delete playerPair.second;
        }
    }

    void Game::update()
    {
        // update units
        this->unitManager->updateUnits();

        // send updates to each player
        for (const auto &playerPair : playerMap) {
            if (playerPair.second != nullptr)
                this->unitManager->sendUnitUpdates(playerPair.second);
        }

        // clean units vars
        this->unitManager->cleanAllUnits();
    }

    void Game::addPlayer(std::string token, uWS::WebSocket<1> *ws)
    {
        Player* p = new Player(token, ws);
        playerMap.insert(std::pair<std::string, Player *>(token, p));
        this->tileManager->sendGrid(p);
        this->unitManager->sendUnits(p);
    }
}