//
// Created by connor on 11/23/18.
//

#include "Projectile.h"
#include "../../Math/MathUtils.h"
#include "../Game.h"
#include "UnitManager.h"

namespace DsprGameServer {
    Projectile::Projectile(Game *game, int fromX, int fromY, int toX, int toY, int index, Unit *unit) {
        this->game = game;
        this->fromCoord = new Point(fromX,fromY);
        this->toCoord = new Point(toX,toY);
        this->distance = MathUtils::Distance(fromX, fromY, toX, toY);
        this->index = index;
        this->unit = unit;

        switch(this->index)
        {
            case 0:
                this->speed = 3;
                this->damage = MathUtils::getRandom(15, 20);
                break;
        }
    }

    Projectile::~Projectile() {
        delete fromCoord;
        delete toCoord;
    }

    void Projectile::update() {
        this->distance -= this->speed;
        if (this->distance <= 0)
        {
            this->done = true;

            this->game->unitManager->damageUnitAtPoint(toCoord->x, toCoord->y, this->damage);
        }
    }
}

