#pragma once

//
// Created by connor on 7/31/18.
//

#include <map>
#include <queue>
#include <list>
#include "Unit.h"
#include "Player.h"

namespace DsprGameServer {
    class UnitManager {
    public:
        UnitManager();
        ~UnitManager();
        void sendUnits(DsprGameServer::Player *pPlayer);
        Unit* createUnit();
        void receiveUnitOrder(const std::list<int>& idList, int tileX, int tileY);
    private:
        std::map<int, Unit*> unitMap;
        std::queue<int> unusedIds;
    };
}