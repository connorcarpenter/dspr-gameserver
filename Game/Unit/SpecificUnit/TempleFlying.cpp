//
// Created by connor on 11/2/18.
//

#include "TempleFlying.h"
#include "../../../Pathfinding/AStarPathfinder.h"
#include "../OrderGroup.h"
#include "../UnitManager.h"
#include "../UnitTemplateCatalog.h"

namespace DsprGameServer
{
    TempleFlying::TempleFlying(DsprGameServer::Unit *unit) {
        this->masterUnit = unit;
    }

    void TempleFlying::specialAction(int actionIndex, int x, int y) {
        switch(actionIndex)
        {
            case 0:
            {
                //Land Action Start
                auto path = this->masterUnit->game->pathfinder->findPath(this->masterUnit->position->x, this->masterUnit->position->y, x, y, false);
                if (path != nullptr)
                {
                    auto newOrderGroup = std::make_shared<OrderGroup>(this->masterUnit->game, UnitOrderType::Move);
                    this->masterUnit->setOrderGroup(newOrderGroup);
                    newOrderGroup->setPath(path);
                    this->masterUnit->startPath();
                }

                auto landOrder = UnitOrder(UnitOrderType::SpecialAction, 1);
                this->masterUnit->queuedOrders.push_front(landOrder);
            }
                break;
        }
    }

    void TempleFlying::specialAction(int actionIndex) {
        switch(actionIndex)
        {
            case 1:
            {
                //Land Action End
                this->masterUnit->game->unitManager->changeUnitsTemplate(this->masterUnit, this->masterUnit->game->unitTemplateCatalog->templeBuilding);
            }
                break;
        }
    }
}

