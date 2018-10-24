#pragma once

//
// Created by connor on 9/1/18.
//

#include <list>
#include "../../Math/Point.h"

namespace DsprGameServer
{
    class IsoBox
    {

    public:

        IsoBox(unsigned int width, unsigned int height);
        ~IsoBox();

        std::list<Point*> coordList;

    private:
        unsigned int width;
        unsigned int height;
    };
}