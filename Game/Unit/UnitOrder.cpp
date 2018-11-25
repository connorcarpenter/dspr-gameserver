//
// Created by connor on 11/24/18.
//

#include "UnitOrder.h"

namespace DsprGameServer {

    UnitOrder::UnitOrder(UnitOrderType unitOrderType) {
        this->orderType = unitOrderType;
    }

    UnitOrder::UnitOrder(UnitOrderType unitOrderType, int toX, int toY) {
        this->orderType = unitOrderType;
        this->toX = toX;
        this->toY = toY;
    }

    UnitOrder::UnitOrder(UnitOrderType unitOrderType, int targetId) {
        this->orderType = unitOrderType;
        this->targetId = targetId;
    }
}