#pragma once

//
// Created by connor on 8/15/18.
//

#include "Unit.h"
#include "UnitOrder.h"
#include "../Item/Item.h"

namespace DsprGameServer
{
    class Unit;
    class Game;

    class OrderGroup
    {
    public:

        OrderGroup(Game* game, UnitOrderType order);

        void addUnit(Unit* unit);
        void removeUnit(Unit *unit);
        int getNumberUnits();
        void recalculatePathIfTargetMoved();
        void setTargetUnit(Unit* targetUnit);
        void setPath(std::shared_ptr<DsprGameServer::Path> path);
        void getMinAndMaxDisInGroup(int& minDis, int& maxDis);
        int getUnitsArrived();
        void unitArrived();
        void unitUnarrived();
        bool isAttacking();
        bool targetHasMoved();
        Unit *getTargetUnit();
        void setTargetItem(DsprGameServer::Item *targetItem);
        Item *getTargetItem();

        int targetUnitId = 0;
        UnitOrderType orderIndex;
        std::shared_ptr<DsprGameServer::Path> path = nullptr;

    private:

        bool targetOffPath();
        void targetUpdatePosition();

        std::list<int> unitIds;
        int unitsArrived = 0;
        Point lastTargetPosition = DsprGameServer::Point(0, 0);
        Game* game = nullptr;
        int targetItemId;
    };
}
