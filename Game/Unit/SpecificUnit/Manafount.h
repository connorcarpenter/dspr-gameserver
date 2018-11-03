#pragma once

//
// Created by connor on 11/2/18.
//

#include "SpecificUnit.h"
#include "../Unit.h"

namespace DsprGameServer {
    class Manafount : public SpecificUnit {

    public:
        Manafount(DsprGameServer::Unit *unit);

        void addGatheringUnit();
        void removeGatheringUnit();
        int gatherRate = 0;
    private:
        int unitsGathering = 0;
        int getNewGatherRate();
        Unit* masterUnit = nullptr;
    };
}