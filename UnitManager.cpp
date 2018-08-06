//
// Created by connor on 7/31/18.
//

#include <sstream>
#include "UnitManager.h"
#include "MathUtils.h"
#include "GameServer.h"
#include "Pathfinding/AStarPathfinder.h"

namespace DsprGameServer
{
    UnitManager::UnitManager(TileManager* tileManager)
    {
        this->tileManager = tileManager;

        this->pathfinder = new AStarPathfinder(tileManager);

        createUnit();
    }

    UnitManager::~UnitManager()
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            delete unit;
        }

        delete this->pathfinder;
    }

    void UnitManager::sendUnits(DsprGameServer::Player* player)
    {
        for(const auto& unitPair : unitMap)
        {
            int id = unitPair.first;
            Unit* unit = unitPair.second;

            std::stringstream msg;
            msg << "unit/1.0/create|" << id << "," << unit->position->x << "," << unit->position->y << "\r\n";
            GameServer::get().queueMessage(player->getWs(), msg.str());
        }
    }

    Unit* UnitManager::createUnit()
    {
        Unit* newUnit = new Unit((int) unitMap.size(), (MathUtils::getRandom(tileManager->width))*2, (MathUtils::getRandom(tileManager->height))*2);
        unitMap.insert(std::pair<int, Unit*>(newUnit->id, newUnit));
    }

    void UnitManager::receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unitPositionsList.emplace_back(std::pair<int,int>(unit->position->x, unit->position->y));
        }

        auto path = this->pathfinder->findPath(unitPositionsList, (int) unitPositionsList.size(), tileX, tileY);

        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unit->startPath(path);
        }
    }

    void UnitManager::updateUnits()
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            unit->update();
        }
    }

    void UnitManager::sendUnitUpdates(Player* player)
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            unit->sendUpdate(player);
        }
    }

    void UnitManager::cleanAllUnits()
    {
        for(const auto& unitPair : unitMap)
        {
            unitPair.second->cleanAllVars();
        }
    }
}