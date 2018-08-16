//
// Created by connor on 8/15/18.
//

#include "MoveGroup.h"
#include "Unit.h"

namespace DsprGameServer {

    FloatPoint *MoveGroup::getAverageGroupPosition() {
        if (units.size() == 0)
        {
            return nullptr;
        }

        auto output = new FloatPoint(0,0);
        for (auto unit : units)
        {
            output->Add((float) unit->position->x / units.size(), (float) unit->position->y / units.size());
        }
        return output;
    }

    void MoveGroup::addUnit(Unit* unit) {
        units.push_back(unit);
    }

    void MoveGroup::removeUnit(Unit* unit){
        units.remove(unit);
    }

    int MoveGroup::getNumberUnits() {
        return units.size();
    }

    int MoveGroup::getAcceptableHeat() {
        if (getNumberUnits() == 1) return 0;
        if (getNumberUnits() <= 9) return 13;
        return 23;
    }
}