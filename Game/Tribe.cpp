//
// Created by connor on 8/27/18.
//

#include "Tribe.h"

namespace DsprGameServer
{

    void Tribe::setEnemy(Tribe* otherTribe) {
        this->enemies.insert(otherTribe);
    }
}