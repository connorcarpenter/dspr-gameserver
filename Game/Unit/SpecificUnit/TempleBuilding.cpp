//
// Created by connor on 11/2/18.
//

#include "TempleBuilding.h"
#include "../UnitTemplateCatalog.h"
#include "../UnitManager.h"

namespace DsprGameServer
{
    TempleBuilding::TempleBuilding(DsprGameServer::Unit *unit) {
        this->masterUnit = unit;
    }

    void TempleBuilding::update() {
        if (this->animationFrames > 0)
        {
            this->animationFrames--;
            if (this->animationFrames == 0)
            {
                if (this->liftingOff)
                {
                    this->masterUnit->game->unitManager->changeUnitsTemplate(this->masterUnit, this->masterUnit->game->unitTemplateCatalog->templeFlying);
                    return;
                }
            }
        }
    }

    void TempleBuilding::specialAction(int actionIndex) {
        switch(actionIndex)
        {
            case 0:
            {
                //Lift off
                this->liftingOff = true;
                this->animationFrames = 20;
            }
                break;
            default:
            {
                int i = 1/0; // ?
            }
                break;
        }
    }


}

