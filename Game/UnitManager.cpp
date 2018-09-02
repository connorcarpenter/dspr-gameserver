//
// Created by connor on 7/31/18.
//

#include <sstream>
#include "UnitManager.h"
#include "../Math/MathUtils.h"
#include "../GameServer.h"
#include "../Pathfinding/AStarPathfinder.h"
#include "OrderGroup.h"
#include "TribeManager.h"

namespace DsprGameServer
{
    UnitManager::UnitManager(Game *game)
    {
        this->game = game;

        this->unitGridArrayA = initializeUnitGridArray(this->game->tileManager->width, this->game->tileManager->height);
        this->unitGridArrayB = initializeUnitGridArray(this->game->tileManager->width, this->game->tileManager->height);
    }

    void UnitManager::initializeFirstUnits(){
        for (int i = 0; i<3; i++)
            for (int j = 0; j<4;j++)
                createUnit((6 + i) * 2, (6 + j) * 2, this->game->tribeManager->tribeA);

        for(int i=0;i<10;i++){
            createUnit((MathUtils::getRandom(this->game->tileManager->width-30)+10) * 2,
                       (MathUtils::getRandom(this->game->tileManager->height-30)+10) * 2,
                       this->game->tribeManager->tribeB);
        }
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
            msg << "unit/1.0/create|" << id << "," << unit->position->x << "," << unit->position->y << "," << unit->tribe->index << "\r\n";
            GameServer::get().queueMessage(player->getWs(), msg.str());
        }
    }

    Unit* UnitManager::createUnit(int x, int y, Tribe* tribe)
    {
        Unit* newUnit = new Unit(this->game, (int) unitMap.size(), tribe, x, y);
        unitMap.insert(std::pair<int, Unit*>(newUnit->id, newUnit));
    }

    void UnitManager::receiveMoveOrder(const std::list<int> &idList, int tileX, int tileY)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::Move);

        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
            unit->setOrderGroup(newOrderGroup);
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, tileX, tileY);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                Unit *unit = unitMap.at(i);
                unit->startPath();
            }
        }
    }

    void UnitManager::receiveFollowOrder(const std::list<int> &idList, int targetUnitId)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::Follow);
        auto targetUnit = unitMap.at(targetUnitId);
        newOrderGroup->setTargetUnit(targetUnit);


        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unitPositionsList.emplace_back(std::pair<int,int>(unit->position->x, unit->position->y));
            unit->setOrderGroup(newOrderGroup);
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x, targetUnit->position->y);
        if (path != nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                Unit *unit = unitMap.at(i);
                unit->startPath();
            }
        }
    }

    void UnitManager::receiveAttackTargetOrder(const std::list<int> &idList, int targetUnitId)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::AttackTarget);
        auto targetUnit = unitMap.at(targetUnitId);
        newOrderGroup->setTargetUnit(targetUnit);

        for (const auto& i : idList)
        {
            Unit* unit = unitMap.at(i);
            unitPositionsList.emplace_back(std::pair<int,int>(unit->position->x, unit->position->y));
            unit->setOrderGroup(newOrderGroup);
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x, targetUnit->position->y);
        if (path != nullptr)
        {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                Unit *unit = unitMap.at(i);
                unit->startPath();
            }
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

    Unit** UnitManager::initializeUnitGridArray(int width, int height) {
        Unit** output = new Unit*[width * height];
        for (int j = 0; j < height; j += 1)
        {
            for (int i = 0;i< width; i+=1)
            {
                output[(j*width)+i] = nullptr;
            }
        }
        return output;
    }

    int UnitManager::getGridIndex(int x, int y) {
        if (x < 0 || y < 0 || x >= this->game->tileManager->width*2 || y >= this->game->tileManager->height*2) return -1;

        if (x % 2 == 0 && y % 2 == 0) return 0;
        if ((x+1) % 2 == 0 && (y+1) % 2 == 0) return 1;
        return -1;
    }

    int UnitManager::getTileIndex(int gridIndex, int x, int y) {
        if (gridIndex == 0)
        {
            int xsmall = x / 2;
            int ysmall = y / 2;
            return (ysmall * this->game->tileManager->width) + xsmall;
        }
        else
        {
            int xsmall = (x-1) / 2;
            int ysmall = (y-1) / 2;
            return (ysmall * this->game->tileManager->width) + xsmall;
        }
    }

    Unit* UnitManager::getUnitFromGrid(int x, int y){
        int gridIndex = getGridIndex(x, y);
        if (gridIndex == -1) return nullptr;
        int tileIndex = getTileIndex(gridIndex, x, y);
        if (gridIndex == 0)
        {
            return this->unitGridArrayA[tileIndex];
        }
        else
        {
            return this->unitGridArrayB[tileIndex];
        }
    }

    void UnitManager::setUnitInGrid(int x, int y, Unit* unit){
        int gridIndex = getGridIndex(x, y);
        if (gridIndex == -1) return;
        int tileIndex = getTileIndex(gridIndex, x, y);
        if (gridIndex == 0)
        {
            this->unitGridArrayA[tileIndex] = unit;
        }
        else
        {
            this->unitGridArrayB[tileIndex] = unit;
        }
    }

    void UnitManager::removeUnitFromGrid(Unit *unit) {
        setUnitInGrid(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y, nullptr);
    }

    void UnitManager::addUnitToGrid(Unit *unit) {
        setUnitInGrid(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y, unit);
    }
}