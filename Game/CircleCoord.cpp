//
// Created by connor on 9/1/18.
//

#include "CircleCoord.h"
#include "../Math/MathUtils.h"

namespace DsprGameServer {

    CircleCoord::CircleCoord(int x, int y) {
        this->x = x;
        this->y = y;
        this->distanceFromCenter = MathUtils::Distance(0,0,x,y);
    }
}