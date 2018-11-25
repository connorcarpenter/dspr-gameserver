//
// Created by connor on 8/15/18.
//

#include "OrderGroup.h"
#include "Unit.h"
#include "../../Pathfinding/Path.h"
#include "../../Pathfinding/AStarPathfinder.h"
#include "UnitManager.h"
#include "../Item/Item.h"
#include "../Item/ItemManager.h"

namespace DsprGameServer {

    OrderGroup::OrderGroup(Game *game, UnitOrderType order) {
        this->game = game;
        this->orderIndex = order;
    }

    void OrderGroup::addUnit(Unit* unit) {
        unitIds.push_back(unit->id);
    }

    void OrderGroup::removeUnit(Unit* unit){
        unitIds.remove(unit->id);
        if (!unit->followingPath) this->unitsArrived -= 1;
    }

    int OrderGroup::getNumberUnits() {
        return unitIds.size();
    }

    void OrderGroup::unitArrived(){
        this->unitsArrived += 1;
    }

    void OrderGroup::unitUnarrived(){
        this->unitsArrived -= 1;
    }

    bool OrderGroup::targetHasMoved() {
        auto targetUnit = this->getTargetUnit();
        if (targetUnit == nullptr) return false;
        return !this->lastTargetPosition.Equals(targetUnit->position);
    }

    bool OrderGroup::targetOffPath() {
        if (this->path == nullptr) return true;
        return this->path->getTile(this->getTargetUnit()->position->x, this->getTargetUnit()->position->y) == nullptr;
    }

    void OrderGroup::targetUpdatePosition(){
        this->lastTargetPosition.Set(this->getTargetUnit()->position);
    }

    void OrderGroup::recalculatePathIfTargetMoved()
    {
        if (targetHasMoved())
        {
            if (targetOffPath())
            {
                std::list<std::pair<int, int>> unitPositionsList;

                for (const auto& unitId : unitIds)
                {
                    auto unit = this->game->unitManager->getUnitWithId(unitId);
                    if (unit == nullptr) continue;
                    unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
                }

                auto targetUnit = this->getTargetUnit();
                auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                             targetUnit->position->y, orderIndex == AttackTarget);
                if (path != nullptr)
                {
                    this->setPath(path);

                    for (const auto &unitId : unitIds) {
                        auto unit = this->game->unitManager->getUnitWithId(unitId);
                        if (unit == nullptr) continue;
                        unit->startPath();
                    }

                    this->unitsArrived = 0;
                    targetUpdatePosition();
                }
            }
        }
    }

    void OrderGroup::setTargetUnit(Unit *targetUnit) {
        if (targetUnit == nullptr){
            this->targetUnitId = -1;
        }else {
            this->targetUnitId = targetUnit->id;
            this->lastTargetPosition.Set(targetUnit->position);
        }

        for (const auto &unitId : unitIds) {
            auto unit = this->game->unitManager->getUnitWithId(unitId);
            if (unit == nullptr) continue;
            if (unit->syncedTargetUnitId->obj()->Get() != this->targetUnitId)
            {
                unit->syncedTargetUnitId->dirtyObj()->Set(this->targetUnitId);
            }
        }
    }

    void OrderGroup::setTargetItem(Item* targetItem) {
        if (targetItem == nullptr){
            this->targetItemId = -1;
        }else {
            this->targetItemId = targetItem->id;
        }

//        for (const auto &unitId : unitIds) {
//            auto unit = this->game->unitManager->getUnitWithId(unitId);
//            if (unit == nullptr) continue;
//            if (unit->syncedTargetUnitId->obj()->Get() != this->targetUnitId)
//            {
//                unit->syncedTargetUnitId->dirtyObj()->Set(this->targetUnitId);
//            }
//        }
    }

    void OrderGroup::setPath(std::shared_ptr<DsprGameServer::Path> path) {
        this->path = path;
    }

    void OrderGroup::getMinAndMaxDisInGroup(int& minDis, int& maxDis) {
        if (this->unitIds.size() <= 0) return;

        minDis = INT_MAX;
        maxDis = INT_MIN;
        for (const auto &unitId : unitIds) {
            auto unit = this->game->unitManager->getUnitWithId(unitId);
            if (unit == nullptr) continue;
            if(unit->disToEnd < minDis)minDis = unit->disToEnd;
            if(unit->disToEnd > maxDis)maxDis = unit->disToEnd;
        }
    }

    int OrderGroup::getUnitsArrived() {
        return this->unitsArrived;
    }

    bool OrderGroup::isAttacking() {
        return orderIndex == AttackTarget || orderIndex == AttackMove;
    }

    Unit *OrderGroup::getTargetUnit() {
        return this->game->unitManager->getUnitWithId(this->targetUnitId);
    }

    Item *OrderGroup::getTargetItem() {
        return this->game->itemManager->getItem(this->targetItemId);
    }
}