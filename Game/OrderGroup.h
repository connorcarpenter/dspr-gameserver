//
// Created by connor on 8/15/18.
//

#include "Unit.h"

namespace DsprGameServer {
    class Unit;
    class Game;

    class OrderGroup {
    public:

        OrderGroup(Game* game, UnitOrder order);

        void addUnit(Unit* unit);
        void removeUnit(Unit *unit);
        int getNumberUnits();
        int getAcceptableDisToEnd();
        void unitArrived();
        void recalculatePathIfTargetMoved();
        void setTargetUnit(Unit* targetUnit);
        void setPath(std::shared_ptr<DsprGameServer::Path> path);
        void getMinAndMaxDisInGroup(int& minDis, int& maxDis);
        int getUnitsArrived();

        Unit* targetUnit = nullptr;
        UnitOrder orderIndex;
        std::shared_ptr<DsprGameServer::Path> path = nullptr;

        int getAcceptableTilesToEnd();

        void unitUnarrived();

    private:

        std::list<Unit*> units;
        int unitsArrived = 0;
        Point lastTargetPosition = DsprGameServer::Point(0, 0);
        Game* game = nullptr;

    };
}