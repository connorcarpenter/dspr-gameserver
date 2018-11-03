//
// Created by connor on 11/2/18.
//

#include "Manafount.h"

namespace DsprGameServer
{
    Manafount::Manafount(DsprGameServer::Unit *unit) {
        this->masterUnit = unit;
    }

    void Manafount::addGatheringUnit() {
        this->unitsGathering ++;
        this->gatherRate = getNewGatherRate();
    }

    void Manafount::removeGatheringUnit() {
        this->unitsGathering --;
        this->gatherRate = getNewGatherRate();
    }

    int Manafount::getNewGatherRate() {
        if (this->unitsGathering<=3) return 4;
        return 3;
    }
}

