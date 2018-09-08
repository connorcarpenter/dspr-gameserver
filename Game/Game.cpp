//
// Created by connor on 7/23/18.
//

#include <sstream>
#include "Game.h"
#include "TileManager.h";
#include "UnitManager.h";
#include "TribeManager.h";
#include "../Pathfinding/AStarPathfinder.h"
#include "../Pathfinding/SimplePathfinder.h"
#include "../PlayerData.h"
#include "FogManager.h"
#include "../GameServer.h"

namespace DsprGameServer
{
    Game::Game()
    {
        this->tileManager = new TileManager(this, 64, 64);
        AStarPathfinder::setMapWidth(64);
        this->fogManager = new FogManager(this);
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
        delete this->fogManager;
    }

    void Game::update()
    {
        // update units
        this->unitManager->updateUnits();

        // send updates to each player
        for (const auto &playerData : this->playerDataSet) {
            this->unitManager->sendUnitUpdates(playerData);
        }

        // clean units vars
        this->unitManager->cleanAllUnits();

        // delete queued units
        this->unitManager->deleteUnits();

        // send deletes to each player
        for (const auto &playerData : this->playerDataSet) {
            this->unitManager->sendUnitDeletes(playerData);
        }

        this->unitManager->finishSendUnitDeletes();
    }

    void Game::addPlayer(const std::string& token, PlayerData *playerData)
    {
        playerData->setToken(token);
        playerData->setCurrentGame(this);

        this->playerDataSet.insert(playerData);

        auto freeTribe = this->tribeManager->getFreeTribe();
        this->tribeManager->assignTribeToPlayer(freeTribe, playerData);
        this->sendPlayerTribeIndex(playerData, freeTribe);

        this->tileManager->sendGrid(playerData);

        this->unitManager->addPlayer(playerData);
        this->unitManager->initSendAllUnits(playerData);
    }

    void Game::removePlayer(PlayerData *playerData) {
        this->playerDataSet.erase(playerData);
        this->tribeManager->freeTribeFromPlayer(playerData->getTribe(), playerData);
        this->unitManager->removePlayer(playerData);
    }

    void Game::sendPlayerTribeIndex(PlayerData *playerData, Tribe *tribe) {
        std::stringstream msg;
        msg << "tribe/1.0/set|" << tribe->index << "\r\n";
        GameServer::get().queueMessage(playerData, msg.str());
    }
}