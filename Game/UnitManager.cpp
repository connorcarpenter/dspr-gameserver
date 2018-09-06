//
// Created by connor on 7/31/18.
//

#include <sstream>
#include <iostream>
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

        this->unitGrid = new PtrIsoGrid<Unit*>();
        this->unitGrid->initialize(this->game->tileManager->width * 2, this->game->tileManager->height * 2);

        this->endPosGrid = new PrimIsoGrid<bool>();
        this->endPosGrid->initialize(this->game->tileManager->width * 2, this->game->tileManager->height * 2);
        this->endPosGrid->forEachCoord([&](int x, int y){
           this->endPosGrid->set(x,y,false);
        });
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

        delete unitGrid;
        delete endPosGrid;
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
            if (unitMap.count(i) != 0){
                Unit* unit = unitMap.at(i);
                unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, tileX, tileY, false);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveAttackMoveOrder(const std::list<int> &idList, int tileX, int tileY)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::AttackMove);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                unitPositionsList.emplace_back(
                        std::pair<int, int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, tileX, tileY, false);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveFollowOrder(const std::list<int> &idList, int targetUnitId)
    {
        if (unitMap.count(targetUnitId) == 0) return;

        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::Follow);

        auto targetUnit = unitMap.at(targetUnitId);
        newOrderGroup->setTargetUnit(targetUnit);


        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                unitPositionsList.emplace_back(std::pair<int, int>(unit->position->x, unit->position->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                     targetUnit->position->y, false);
        if (path != nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveAttackTargetOrder(const std::list<int> &idList, int targetUnitId)
    {
        if (unitMap.count(targetUnitId) == 0) return;

        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::AttackTarget);
        auto targetUnit = unitMap.at(targetUnitId);
        newOrderGroup->setTargetUnit(targetUnit);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                unitPositionsList.emplace_back(std::pair<int, int>(unit->position->x, unit->position->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                     targetUnit->position->y, true);
        if (path != nullptr)
        {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    unit->startPath();
                }
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

    void UnitManager::setUnitInGrid(int x, int y, Unit* unit){
        return this->unitGrid->set(x,y, unit);
    }

    Unit* UnitManager::getUnitFromGrid(int x, int y){
        return this->unitGrid->get(x,y);
    }

    void UnitManager::removeUnitFromGrid(Unit *unit) {
        setUnitInGrid(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y, nullptr);
    }

    void UnitManager::addUnitToGrid(Unit *unit) {
        setUnitInGrid(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y, unit);
    }

    bool UnitManager::getEndPosAtCoord(int x, int y) {
        return this->endPosGrid->get(x, y);
    }

    void UnitManager::addEndPosAtCoord(int x, int y) {
        this->endPosGrid->set(x, y, true);
    }

    void UnitManager::removeEndPosAtCoord(int x, int y) {
        this->endPosGrid->set(x, y, false);
    }

    void UnitManager::deleteUnits() {
        for(auto unit : this->unitsToDelete)
        {
            std::cout << "deleting unit, w/ id: " << unit->id << "\r\n";

            //clean up
            this->removeUnitFromGrid(unit);
            this->unitMap.erase(unit->id);

            //setup sending the deletions
            this->unitDeletionsToSend.emplace(unit->id);

            //actually delete
            delete unit;

            std::cout << "deleted unit: " << unit << "\r\n";
        }

        this->unitsToDelete.clear();
    }

    void UnitManager::sendUnitDeletes(Player* player)
    {
        for(auto unitId : this->unitDeletionsToSend)
        {
            std::stringstream msg;
            msg << "unit/1.0/delete|" << unitId << "|1" << "\r\n";
            GameServer::get().queueMessage(player->getWs(), msg.str());
        }

        this->unitDeletionsToSend.clear();
    }

    void UnitManager::queueUnitForDeletion(Unit* deletedUnit){
        this->unitsToDelete.emplace(deletedUnit);
    }

    Unit *UnitManager::getUnitWithId(int unitId) {
        if (this->unitMap.count(unitId) <= 0)return nullptr;
        return this->unitMap.at(unitId);
    }
}