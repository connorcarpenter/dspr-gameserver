#pragma once

//
// Created by connor on 8/2/18.
//

#include <string>
#include "../Pathfinding/AStarPathfinder.h"

namespace DsprGameServer
{
    class Point
    {
    public:
        Point(int x, int y);
        int x;
        int y;

        bool Equals(const Point *otherPoint);

        void Set(const Point *otherPoint);

        void Set(int x, int y);

        std::string serialize();

        void Add(int x, int y);

        bool
        operator ==(const Point & obj) const
        {
            if (x == obj.x && y == obj.y)
                return true;
            else
                return false;
        }
    };
}

namespace std
{
    template<>
    struct hash<DsprGameServer::Point>
    {
        size_t
        operator()(const DsprGameServer::Point & obj) const
        {
            return hash<int>()(obj.x + (obj.y * DsprGameServer::AStarPathfinder::getMapWidth()));
        }
    };
}