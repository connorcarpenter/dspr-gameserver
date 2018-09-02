//
// Created by connor on 9/1/18.
//

#include "CircleCache.h"

namespace DsprGameServer {

    CircleCache::CircleCache() {}

    CircleCache::~CircleCache() {
        for(const auto& circlePair : circleMap)
        {
            auto circle = circlePair.second;
            delete circle;
        }
    }

    Circle *CircleCache::getCircle(int radius) {
        if (this->circleMap.count(radius) > 0)
            return this->circleMap.at(radius);
        auto newCircle = new Circle(radius);
        this->circleMap.emplace(radius, newCircle);
        return newCircle;
    }


}