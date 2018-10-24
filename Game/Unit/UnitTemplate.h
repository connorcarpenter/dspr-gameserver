#pragma once

//
// Created by connor on 9/12/18.
//

namespace DsprGameServer
{
    class UnitTemplate
    {

    public:

        UnitTemplate(int index);

        void setWalkSpeed(int straightTicks, int diagTicks);

        int index;
        int sight;
        int acquisition;
        int walkMax = 0;
        int walkSpeedStraight = 0;
        int walkSpeedDiagonal = 0;
        unsigned int tileWidth = 1;
        unsigned int tileHeight = 1;
    };
}