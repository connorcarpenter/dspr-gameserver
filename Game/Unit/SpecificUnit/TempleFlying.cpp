//
// Created by connor on 11/2/18.
//

#include "TempleFlying.h"

namespace DsprGameServer
{
    TempleFlying::TempleFlying(DsprGameServer::Unit *unit) {
        this->masterUnit = unit;
    }

    void TempleFlying::specialAction(int actionIndex, int x, int y) {
        switch(actionIndex)
        {
            case 0:
            {
                int i = 12;
            }
                break;
        }
    }
}

