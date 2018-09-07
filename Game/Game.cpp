//
// Created by connor on 7/23/18.
//

#include "Game.h"
#include "TileManager.h";
#include "UnitManager.h";
#include "TribeManager.h";
#include "../Pathfinding/AStarPathfinder.h"
#include "../Pathfinding/SimplePathfinder.h"
#include "../PlayerData.h"

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
    }

    void Game::update()
    {
        // update units
        this->unitManager->updateUnits();

        // delete queued units
        this->unitManager->deleteUnits();

        // send deletes to each player
        for (const auto &playerData : this->playerDataSet) {
            this->unitManager->sendUnitDeletes(playerData);
        }

        // send updates to each player
        for (const auto &playerData : this->playerDataSet) {
            this->unitManager->sendUnitUpdates(playerData);
        }

        // clean units vars
        this->unitManager->cleanAllUnits();
    }

    void Game::addPlayer(const std::string& token, PlayerData *playerData)
    {
        playerData->setToken(token);
        playerData->setCurrentGame(this);

        this->playerDataSet.insert(playerData);
        this->tileManager->sendGrid(playerData);
        this->unitManager->sendUnits(playerData);
    }

    void Game::removePlayer(PlayerData *playerData) {
        this->playerDataSet.erase(playerData);
    }
}