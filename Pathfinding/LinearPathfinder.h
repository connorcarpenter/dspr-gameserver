#pragma once

//
// Created by connor on 8/4/18.
//

#include "Pathfinder.h"

namespace DsprGameServer
{
    class LinearPathfinder : public Pathfinder
    {
    public:
        virtual std::shared_ptr<Path> findPath(const std::list<std::pair<int,int>>& unitPositions, int unitNumber, int targetX, int targetY);
    };
}