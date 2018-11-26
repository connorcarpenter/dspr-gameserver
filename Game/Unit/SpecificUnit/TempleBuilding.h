#pragma once

//
// Created by connor on 11/2/18.
//

#include "SpecificUnit.h"
#include "../Unit.h"

namespace DsprGameServer {
    class TempleBuilding : public SpecificUnit {

    public:
        TempleBuilding(DsprGameServer::Unit *unit);
        void specialAction(int actionIndex) override;

    private:
        Unit* masterUnit = nullptr;
    };
}