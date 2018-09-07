//
// Created by connor on 9/1/18.
//

#include "Circle.h"

namespace DsprGameServer
{
    bool compareCoords (const CircleCoord* first, const CircleCoord* second)
    {
        return first->distanceFromCenter < second->distanceFromCenter;
    }

    Circle::Circle(int radius) {
        this->radius = radius;

        for (int x = -radius; x <= radius; x++)
        {
            for (int y = -radius; y <= radius; y++)
            {
                if (!((x % 2 == 0 && y % 2 == 0) || ((x+1) % 2 == 0 && (y+1) % 2 == 0)))continue;

                auto newCoord = new CircleCoord(x,y);
                if (newCoord->distanceFromCenter > radius)
                {
                    delete newCoord;
                    continue;
                }

                coordList.push_back(newCoord);
            }
        }

        coordList.sort(compareCoords);
    }

    Circle::~Circle() {
        for(auto coord : coordList)
            delete coord;
    }
}