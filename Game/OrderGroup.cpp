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
    }

    int OrderGroup::getNumberUnits() {
        return units.size();
    }

    void OrderGroup::unitArrived(){
        this->unitsArrived += 1;
    }

    int OrderGroup::getAcceptableHeat() {
        if (this->unitsArrived == 0) return 0;
        if (getNumberUnits() < 9) return 13;
        return 23;
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
                    unitPositionsList.emplace_back(std::pair<int,int>(unit->position->x, unit->position->y));
                }

                auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x, targetUnit->position->y);
                this->setPath(path);

                for (const auto& unit : units)
                {
                    unit->startPath();
                }

                this->unitsArrived = 0;
                this->lastTargetPosition.Set(targetUnit->position);
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
}