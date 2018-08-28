//
// Created by connor on 7/31/18.
//

#include <sstream>
#include "UnitManager.h"
#include "../Math/MathUtils.h"
#include "../GameServer.h"
#include "../Pathfinding/AStarPathfinder.h"
#include "MoveGroup.h"
#include "TribeManager.h"

namespace DsprGameServer
{
    UnitManager::UnitManager(Game *game)
    {
        this->game = game;

        this->pathfinder = new AStarPathfinder(game);

        int i = 0;
        int j = 0;
        //for (int i = 0; i<3; i++)
        //    for (int j = 0; j<4;j++)
                createUnit((6 + i) * 2, (6 + j) * 2, this->game->tribeManager->tribeA);

        //for(int i=0;i<10;i++){
            createUnit((MathUtils::getRandom(this->game->tileManager->width-30)+10) * 2,
                       (MathUtils::getRandom(this->game->tileManager->height-30)+10) * 2,
                       this->game->tribeManager->tribeB);
        //}
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
            msg << "unit/1.0/create|" << id << "," << unit->position->x << "," << unit->position->y << "," << unit->tribe->index << "\r\n";
            GameServer::get().queueMessage(player->getWs(), msg.str());
        }
    }

    Unit* UnitManager::createUnit(int x, int y, Tribe* tribe)
    {
        Unit* newUnit = new Unit(this->game, (int) unitMap.size(), tribe, x, y);
        unitMap.insert(std::pair<int, Unit*>(newUnit->id, newUnit));
    }

    void UnitManager::receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newMoveGroup = std::make_shared<MoveGroup>();

        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unitPositionsList.emplace_back(std::pair<int,int>(unit->position->x, unit->position->y));
            unit->setMoveGroup(newMoveGroup);
        }

        auto path = this->pathfinder->findPath(unitPositionsList, tileX, tileY);

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

    Unit *UnitManager::getUnitWithNextPosition(int x, int y) {
        for(const auto& unitPair : unitMap)
        {
            if (unitPair.second->nextPosition->obj()->x == x && unitPair.second->nextPosition->obj()->y == y)
                return unitPair.second;
        }

        return nullptr;
    }
}