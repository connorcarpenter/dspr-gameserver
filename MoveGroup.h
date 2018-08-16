//
// Created by connor on 8/15/18.
//

#include "Unit.h"

namespace DsprGameServer {
    class Unit;

    class MoveGroup {
    public:
        FloatPoint* getAverageGroupPosition();
        void addUnit(Unit* unit);

        void removeUnit(Unit *unit);
        int getNumberUnits();
        int getAcceptableHeat();

    private:
        std::list<Unit*> units;

    };
}
