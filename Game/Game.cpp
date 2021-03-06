//
// Created by connor on 7/23/18.
//

#include <sstream>
#include "Game.h"
#include "TileManager.h"
#include "Unit/UnitManager.h"
#include "TribeManager.h"
#include "../Pathfinding/AStarPathfinder.h"
#include "../Pathfinding/SimplePathfinder.h"
#include "../PlayerData.h"
#include "FogManager.h"
#include "../GameServer.h"
#include "Unit/UnitTemplateCatalog.h"
#include "EconomyManager.h"
#include "Item/ItemManager.h"
#include "Item/ItemTemplateCatalog.h"
#include "ChatManager.h"
#include "PlaneGeneration/PlaneGenerator.h"

namespace DsprGameServer
{
    Game::Game()
    {
        AStarPathfinder::setMapWidth(80);
        this->planeGenerator = new PlaneGenerator(this);
        this->tileManager = new TileManager(this, 80, 80);
        this->fogManager = new FogManager(this);
        this->economyManager = new EconomyManager(this);
        this->tribeManager = new TribeManager(this);
        this->itemTemplateCatalog = new ItemTemplateCatalog(this);
        this->itemManager = new ItemManager(this);
        this->itemManager->initializeFirstItems();
        this->unitTemplateCatalog = new UnitTemplateCatalog();
        this->unitManager = new UnitManager(this);
        this->unitManager->initializeFirstUnits();
        this->pathfinder = new AStarPathfinder(this);
        this->simplePathfinder = new SimplePathfinder(this);
        this->chatManager = new ChatManager(this);
    }

    Game::~Game()
    {
        delete this->tileManager;
        delete this->unitManager;
        delete this->pathfinder;
        delete this->tribeManager;
        delete this->fogManager;
        delete this->unitTemplateCatalog;
        delete this->chatManager;
    }

    void Game::update()
    {
        // update units
        this->unitManager->updateProjectiles();
        this->unitManager->updateUnits();

        // send updates to each player
        for (const auto &playerData : this->playerDataSet) {
            this->economyManager->sendUpdates(playerData, false);
        }

        for (const auto &playerData : this->playerDataSet) {
            this->unitManager->sendUnitUpdates(playerData);
        }

        for (const auto &playerData : this->playerDataSet) {
            this->itemManager->sendItemUpdates(playerData);
        }

        for (const auto &playerData : this->playerDataSet) {
            this->unitManager->sendProjectiles(playerData);
        }

        // clean units vars
        this->economyManager->clean();
        this->unitManager->cleanAllUnits();
        this->unitManager->cleanProjectiles();

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
        this->tribeManager->sendTribeData(playerData);

        this->tileManager->sendGrid(playerData);

        this->economyManager->sendUpdates(playerData, true);

        this->unitManager->addPlayer(playerData);
        this->unitManager->initSendAllUnits(playerData);

        this->itemManager->addPlayer(playerData);
        this->itemManager->initSendAllItems(playerData);
    }

    void Game::removePlayer(PlayerData *playerData) {
        this->playerDataSet.erase(playerData);
        this->tribeManager->freeTribeFromPlayer(playerData->getTribe(), playerData);
        this->unitManager->removePlayer(playerData);
        this->itemManager->removePlayer(playerData);
    }
}