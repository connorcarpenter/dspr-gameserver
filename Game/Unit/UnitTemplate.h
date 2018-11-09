#pragma once

//
// Created by connor on 9/12/18.
//

#include <set>
#include <functional>

namespace DsprGameServer
{
    class SpecificUnit;
    class Unit;

    class UnitTemplate
    {

    public:

        UnitTemplate(int index);

        void setWalkSpeed(int straightTicks, int diagTicks);

        int maxHealth;
        int index;
        int sight;
        int acquisition;
        int walkMax = 0;
        int walkSpeedStraight = 0;
        int walkSpeedDiagonal = 0;
        unsigned int tileWidth = 1;
        unsigned int tileHeight = 1;
        bool hasConstructionQueue = false;
        std::set<int>* buildableUnits = nullptr;
        int buildTime;
        bool isInvincible = false;
        bool canGather = false;
        bool isGatherable = false;
        bool hasInventory = false;

        std::function<SpecificUnit*(Unit* unit)> createSpecificUnitFunction = nullptr;
    };
}