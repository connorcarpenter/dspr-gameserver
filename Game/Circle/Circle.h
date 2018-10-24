#pragma once

//
// Created by connor on 9/1/18.
//

#include "CircleCoord.h"
#include <list>

namespace DsprGameServer
{
    class Circle
    {

    public:
        Circle(int radius);
        ~Circle();

        std::list<CircleCoord*> coordList;
    private:
        int radius;
    };
}