#pragma once

//
// Created by connor on 11/23/18.
//

#include "../../Math/Point.h"
#include "../Game.h"
#include "Unit.h"

namespace DsprGameServer {
    class Projectile {
    public:
        Projectile(Game *game, int fromX, int fromY, int toX, int toY, int index, Unit *unit);
        ~Projectile();

        Point* fromCoord = nullptr;
        Point* toCoord = nullptr;
        float distance;
        int index;

        void update();

        bool done = false;
        int speed = 1;
        Game *game = nullptr;
        int damage = 0;
        Unit *unit;
    };
}