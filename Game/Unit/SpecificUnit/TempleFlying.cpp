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
#include "../../../GameServer.h"

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
                //Land
                auto newTemplate = this->masterUnit->game->unitTemplateCatalog->templeBuilding;
                if (!landingZoneIsFree(newTemplate))return;

                this->landing = true;
                this->animationFrames = 20;

                DsprMessage::UnitSpecialActionMsgV1 unitSpecialActionMsgV1;
                unitSpecialActionMsgV1.id.set(this->masterUnit->id);
                unitSpecialActionMsgV1.actionIndex.set(1);

                auto clientMsg = unitSpecialActionMsgV1.getToClientMessage();
                auto packedMsg = clientMsg->Pack();
                auto playersAwareOfUnit = this->masterUnit->game->unitManager->getPlayersAwareOfUnit(this->masterUnit);
                for (auto playerData : *playersAwareOfUnit)
                    GameServer::get().queueMessage(playerData, packedMsg);
            }
                break;
        }
    }

    void TempleFlying::update() {
        if (landing)
        {
            if (animationFrames > 0)
            {
                animationFrames--;
            }
            else
            {
                landing = false;

                auto newTemplate = this->masterUnit->game->unitTemplateCatalog->templeBuilding;

                //transform
                if (landingZoneIsFree(newTemplate)) {
                    this->masterUnit->game->unitManager->changeUnitsTemplate(this->masterUnit, newTemplate);
                } else {
                    this->masterUnit->game->unitManager->changeUnitsTemplate(this->masterUnit, this->masterUnit->unitTemplate);
                }
            }
        }
    }

    bool TempleFlying::landingZoneIsFree(UnitTemplate* newTemplate) {
        auto newUnitBase = IsoBoxCache::get().getIsoBox(newTemplate->tileWidth, newTemplate->tileHeight);
        for(auto isoBoxCoord : newUnitBase->coordList)
        {
            auto x = this->masterUnit->position->x + isoBoxCoord->x;
            auto y = this->masterUnit->position->y + isoBoxCoord->y;
            auto tileAtPos = this->masterUnit->game->tileManager->getTileAt(x,y);
            if (tileAtPos == nullptr || !tileAtPos->walkable) return false;

            auto unitAtPos = this->masterUnit->game->unitManager->getUnitFromGrid(x,y);
            if (unitAtPos != nullptr && unitAtPos != this->masterUnit)  return false;

            auto itemAtPos = this->masterUnit->game->itemManager->getItemFromGrid(x,y);
            if (itemAtPos != nullptr)  return false;
        }

        return true;
    }
}

