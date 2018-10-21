#pragma once

//
// Created by connor on 9/1/18.
//

#include "Circle.h"
#include <map>

namespace DsprGameServer {
    class CircleCache {
    public:

        static CircleCache& get() {
            static CircleCache circleCache;
            return circleCache;
        }
        ~CircleCache();
        Circle* getCircle(int radius);
    private:
        CircleCache();
        std::map<int, Circle*> circleMap;
    };
}