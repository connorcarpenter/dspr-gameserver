#pragma once

//
// Created by connor on 11/2/18.
//

#include "SpecificUnit.h"
#include "../Unit.h"

namespace DsprGameServer {
    class TempleFlying : public SpecificUnit {

    public:
        TempleFlying(DsprGameServer::Unit *unit);
        void update() override;
        void specialAction(int actionIndex) override;
        void specialAction(int actionIndex, int x, int y) override;

    private:
        Unit* masterUnit = nullptr;
        bool landing = false;
        int animationFrames = 0;

        bool landingZoneIsFree(UnitTemplate* newTemplate);
    };
}