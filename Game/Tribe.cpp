//
// Created by connor on 8/27/18.
//

#include "Tribe.h"

namespace DsprGameServer
{
    Tribe::Tribe(int index) {
        this->index = index;
    }

    void Tribe::setEnemy(Tribe* otherTribe) {
        this->enemies.insert(otherTribe);
    }

    bool Tribe::isNeutral() {
        return index == -1;
    }
}