#pragma once

//
// Created by connor on 9/10/18.
//

namespace DsprGameServer
{
    enum UnitOrderType
    {
        Move,
        AttackMove,
        Follow,
        AttackTarget,
        Stop,
        Hold,
        Train,
        Gather,
        Pickup,
        ItemSwap,
        ItemDrop,
        ItemGive,
        RallyPoint,
        RallyUnit,
        CancelTrain
    };

    class UnitOrder
    {
    public:
        UnitOrder(UnitOrderType unitOrderType);
        UnitOrder(UnitOrderType unitOrderType, int toX, int toY);
        UnitOrder(UnitOrderType unitOrderType, int targetId);

        UnitOrderType orderType;
        int toX;
        int toY;
    private:
        int targetId;
    };
}