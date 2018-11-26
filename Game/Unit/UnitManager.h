#pragma once

//
// Created by connor on 7/31/18.
//

#include <map>
#include <queue>
#include <list>
#include "Unit.h"
#include "../TileManager.h"
#include "../../Pathfinding/AStarPathfinder.h"
#include "../../PrimIsoGrid.h"
#include "Projectile.h"

namespace DsprGameServer {

    class UnitManager {
    public:
        UnitManager(Game *game);
        ~UnitManager();
        void initSendAllUnits(PlayerData *playerData);
        Unit *createUnit(int x, int y, Tribe *tribe, UnitTemplate *unitTemplate);
        void receiveMoveOrder(const std::list<int> &idList, int tileX, int tileY);
        void receiveFollowOrder(const std::list<int> &idList, int targetUnitId);
        void receiveAttackTargetOrder(const std::list<int> &idList, int targetUnitId);
        void receiveAttackMoveOrder(const std::list<int> &idList, int tileX, int tileY);
        void receiveStopOrder(const std::list<int> &idList);
        void receiveHoldOrder(const std::list<int> &idList);
        void receiveTrainOrder(const std::list<int> &idList, int unitTemplateIndex);
        void receiveGatherOrder(const std::list<int> &idList, int targetUnitId);
        void receivePickupOrder(std::list<int> unitIdList, int targetItemId);
        void receiveItemSwapOrder(int unitId, int beforeSlotIndex, int afterSlotIndex);
        void receiveItemDropOrder(int unitId, int slotIndex, int x, int y);
        void receiveItemGiveOrder(int unitId, int slotIndex, int targetUnitId);
        void receiveRallyPointOrder(const std::list<int> &idList, int tileX, int tileY);
        void receiveRallyUnitOrder(const std::list<int> &idList, int targetId);
        void receiveCancelTrainOrder(std::list<int> idList, int queueIndex);
        void receiveSpecialActionOrder(const std::list<int> &idList, DsprMessage::Array& otherNumbers);

        void updateUnits();
        void deleteUnits();
        void sendUnitUpdates(PlayerData *playerData);
        void cleanAllUnits();
        void removeUnitFromGrid(Unit *unit);
        void addUnitToGrid(Unit *unit);
        Unit *getUnitFromGrid(int x, int y);
        void initializeFirstUnits();
        bool getEndPosAtCoord(int x, int y);
        void addEndPosAtCoord(int x, int y);
        void removeEndPosAtCoord(int x, int y);
        void queueUnitForDeletion(Unit *deletedUnit);
        void sendUnitDeletes(PlayerData *playerData);
        Unit* getUnitWithId(int unitId);
        void finishSendUnitDeletes();
        void addPlayer(PlayerData *playerData);
        void removePlayer(PlayerData *playerData);
        void updateProjectiles();
        void createProjectile(int fromX, int fromY, int toX, int toY, int index, Unit *tribe);
        void damageUnitAtPoint(int fromX, int fromY, int x, int y, int damage);
        void sendProjectiles(PlayerData *const &playerData);
        void cleanProjectiles();
        void changeUnitsTemplate(Unit *unit, UnitTemplate *newTemplate);

    private:
        std::map<int, Unit*> unitMap;
        std::set<Projectile*> projectileSet;
        std::set<Projectile*> projectileCreationSet;
        std::queue<int> unusedIds;
        Game* game = nullptr;

        PtrIsoGrid<Unit*>* unitGrid = nullptr;
        PrimIsoGrid<bool>* endPosGrid = nullptr;

        std::set<Unit*> unitsToDelete;
        std::set<std::pair<int,int>> unitDeletionsToSend;
        std::map<PlayerData*, std::set<Unit*>*> playerToUnitsAwareOfMap;

        void setUnitInGrid(int x, int y, Unit *unit, UnitTemplate *unitTemplate);
        void makePlayerAwareOfUnit(PlayerData *playerData, Unit *unit);
        bool playerIsAwareOfUnit(PlayerData *playerData, Unit *unit);
        void makePlayerUnawareOfUnit(PlayerData *playerData, Unit *unit);
        int getFreeUnitId();


    };
}