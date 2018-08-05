#pragma once

//
// Created by connor on 8/4/18.
//

#include "Path.h"
#include "PathTile.h"

#include <list>
#include <memory>

namespace DsprGameServer
{
    class Pathfinder
    {
    public:
        virtual std::shared_ptr<Path> findPath(const std::list<std::pair<int,int>>& unitPositions, int unitNumber, int targetX, int targetY) = 0;

        static int setMapWidth(int value) { mapWidth = value; }
        static int getMapWidth() { return mapWidth; }
    private:
        static int mapWidth = 0;

    };
}