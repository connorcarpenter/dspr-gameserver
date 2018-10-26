#pragma once

//
// Created by connor on 10/26/18.
//

#include <queue>
#include "UnitTemplate.h"

namespace DsprGameServer {

    class Unit;

    class ConstructionQueue {

    public:
        ConstructionQueue(Unit* masterUnit);
        bool atMaxQueue();

        void enqueue(UnitTemplate *unitTemplate);

        void update();

    private:
        std::queue<UnitTemplate*> unitQueue;
        const int maxQueue = 7;
        int currentBuildTime = 0;
        Unit* masterUnit = nullptr;
    };
}