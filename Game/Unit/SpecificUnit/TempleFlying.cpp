//
// Created by connor on 11/2/18.
//

#include "TempleFlying.h"
#include "../../../Pathfinding/AStarPathfinder.h"
#include "../OrderGroup.h"
#include "../UnitManager.h"
#include "../UnitTemplateCatalog.h"
#include "../../IsoBox/IsoBoxCache.h"
#include "../../Item/ItemManager.h"

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
                auto newTemplate = this->masterUnit->game->unitTemplateCatalog->templeBuilding;

                //check that the space is clear
                auto newUnitBase = IsoBoxCache::get().getIsoBox(newTemplate->tileWidth, newTemplate->tileHeight);

                for(auto isoBoxCoord : newUnitBase->coordList)
                {
                    auto x = this->masterUnit->position->x + isoBoxCoord->x;
                    auto y = this->masterUnit->position->y + isoBoxCoord->y;
                    auto tileAtPos = this->masterUnit->game->tileManager->getTileAt(x,y);
                    if (tileAtPos == nullptr || !tileAtPos->walkable) return;

                    auto unitAtPos = this->masterUnit->game->unitManager->getUnitFromGrid(x,y);
                    if (unitAtPos != nullptr && unitAtPos != this->masterUnit) return;

                    auto itemAtPos = this->masterUnit->game->itemManager->getItemFromGrid(x,y);
                    if (itemAtPos != nullptr) return;
                }

                //transform
                this->masterUnit->game->unitManager->changeUnitsTemplate(this->masterUnit, newTemplate);
            }
                break;
        }
    }
}

