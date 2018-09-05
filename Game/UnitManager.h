#pragma once

//
// Created by connor on 7/31/18.
//

#include <map>
#include <queue>
#include <list>
#include "Unit.h"
#include "../Player.h"
#include "TileManager.h"
#include "../Pathfinding/AStarPathfinder.h"
#include "../PrimIsoGrid.h"

namespace DsprGameServer {

    class UnitManager {
    public:
        UnitManager(Game *game);
        ~UnitManager();
        void sendUnits(DsprGameServer::Player *pPlayer);
        Unit* createUnit(int x, int y, Tribe *tribe);
        void receiveMoveOrder(const std::list<int> &idList, int tileX, int tileY);
        void receiveFollowOrder(const std::list<int> &idList, int targetUnitId);
        void receiveAttackTargetOrder(const std::list<int> &idList, int targetUnitId);
        void receiveAttackMoveOrder(const std::list<int> &idList, int tileX, int tileY);

        void updateUnits();
        void deleteUnits();
        void sendUnitUpdates(Player *player);
        void cleanAllUnits();
        void removeUnitFromGrid(Unit *unit);
        void addUnitToGrid(Unit *unit);
        Unit *getUnitFromGrid(int x, int y);
        void initializeFirstUnits();
        bool getEndPosAtCoord(int x, int y);
        void addEndPosAtCoord(int x, int y);
        void removeEndPosAtCoord(int x, int y);

        void queueUnitForDeletion(Unit *deletedUnit);

        void sendUnitDeletes(Player *player);

        Unit* getUnitWithId(int unitId);

    private:
        std::map<int, Unit*> unitMap;
        std::queue<int> unusedIds;
        Game* game = nullptr;

        PtrIsoGrid<Unit*>* unitGrid = nullptr;
        PrimIsoGrid<bool>* endPosGrid = nullptr;

        std::set<Unit*> unitsToDelete;
        std::set<int> unitDeletionsToSend;

        void setUnitInGrid(int x, int y, Unit *unit);

    };
}