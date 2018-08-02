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
            msg << "unit/1.0/create|" << id << "," << unit->x << "," << unit->y << "\r\n";
            GameServer::get().queueMessage(player->getWs(), msg.str());
        }
    }

    Unit *UnitManager::createUnit()
    {
        Unit* newUnit = new Unit();
        newUnit->id = (int) unitMap.size();
        newUnit->x = (MathUtils::getRandom(10)+2)*2;
        newUnit->y = (MathUtils::getRandom(10)+2)*2;
        unitMap.insert(std::pair<int, Unit*>(newUnit->id, newUnit));
    }

    void UnitManager::receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY)
    {

    }
}