//
// Created by connor on 7/31/18.
//

#include <sstream>
#include "UnitManager.h"
#include "MathUtils.h"
#include "GameServer.h"

namespace DsprGameServer
{
    UnitManager::UnitManager()
    {
        createUnit();
    }

    UnitManager::~UnitManager()
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            delete unit;
        }
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

    Unit *UnitManager::createUnit()
    {
        Unit* newUnit = new Unit((int) unitMap.size(), (MathUtils::getRandom(10)+2)*2, (MathUtils::getRandom(10)+2)*2);
        unitMap.insert(std::pair<int, Unit*>(newUnit->id, newUnit));
    }

    void UnitManager::receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY)
    {
        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unit->moveTarget->x = tileX;
            unit->moveTarget->y = tileY;
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
}