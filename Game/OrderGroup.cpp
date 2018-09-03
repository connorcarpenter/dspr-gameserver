//
// Created by connor on 8/15/18.
//

#include "OrderGroup.h"
#include "Unit.h"
#include "../Pathfinding/Path.h"
#include "../Pathfinding/AStarPathfinder.h"

namespace DsprGameServer {

    OrderGroup::OrderGroup(Game *game, UnitOrder order) {
        this->game = game;
        this->orderIndex = order;
    }

    void OrderGroup::addUnit(Unit* unit) {
        units.push_back(unit);
    }

    void OrderGroup::removeUnit(Unit* unit){
        units.remove(unit);
        if (!unit->followingPath) this->unitsArrived -= 1;
    }

    int OrderGroup::getNumberUnits() {
        return units.size();
    }

    void OrderGroup::unitArrived(){
        this->unitsArrived += 1;
    }

    void OrderGroup::unitUnarrived(){
        this->unitsArrived -= 1;
    }

    void OrderGroup::recalculatePathIfTargetMoved()
    {
        if (!this->lastTargetPosition.Equals(this->targetUnit->position))
        {
            if (this->path->getTile(this->targetUnit->position->x, this->targetUnit->position->y) == nullptr)
            {
                std::list<std::pair<int, int>> unitPositionsList;

                for (const auto& unit : units)
                {
                    unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
                }

                auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                             targetUnit->position->y, false);
                if (path != nullptr)
                {
                    this->setPath(path);

                    for (const auto &unit : units) {
                        unit->startPath();
                    }

                    this->unitsArrived = 0;
                    this->lastTargetPosition.Set(targetUnit->position);
                }
            }
        }
    }

    void OrderGroup::setTargetUnit(Unit *targetUnit) {
        this->targetUnit = targetUnit;
        this->lastTargetPosition.Set(targetUnit->position);
    }

    void OrderGroup::setPath(std::shared_ptr<DsprGameServer::Path> path) {
        this->path = path;
    }

    void OrderGroup::getMinAndMaxDisInGroup(int& minDis, int& maxDis) {
        if (this->units.size() <= 0) return;

        minDis = INT_MAX;
        maxDis = INT_MIN;
        for (const auto &unit : units) {
            if(unit->disToEnd < minDis)minDis = unit->disToEnd;
            if(unit->disToEnd > maxDis)maxDis = unit->disToEnd;
        }
    }

    int OrderGroup::getUnitsArrived() {
        return this->unitsArrived;
    }
}