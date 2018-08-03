#pragma once

//
// Created by connor on 8/2/18.
//

#include <string>

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
    };
}