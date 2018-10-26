//
// Created by connor on 10/26/18.
//

#include "ConstructionQueue.h"
#include "Unit.h"

namespace DsprGameServer
{
    ConstructionQueue::ConstructionQueue(Unit *masterUnit) {
        this->masterUnit = masterUnit;
    }

    bool ConstructionQueue::atMaxQueue()
    {
        return this->unitQueue.size() >= 7;
    }

    void ConstructionQueue::enqueue(UnitTemplate *unitTemplate)
    {
        this->unitQueue.push(unitTemplate);
    }

    void ConstructionQueue::update()
    {
        if (this->unitQueue.size() > 0)
        {
            this->currentBuildTime += 1;
            auto currentlyBuildingUnit = unitQueue.front();
            if (this->currentBuildTime >= currentlyBuildingUnit->buildTime)
            {
                this->currentBuildTime = 0;
                unitQueue.pop();
                this->masterUnit->finishTraining(currentlyBuildingUnit);
            }
        }
    }
}