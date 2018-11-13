#pragma once

//
// Created by connor on 8/1/18.
//

#include <set>
#include "../../Math/Point.h"
#include "Synced.h"
#include "../Game.h"
#include "../../Math/FloatPoint.h"
#include "AnimationState.h"
#include "../../Math/Int.h"
#include "UnitTemplate.h"
#include "ConstructionQueue.h"
#include "../Item/Inventory.h"
#include "SpecificUnit/SpecificUnit.h"
#include "../Item/Item.h"

namespace DsprGameServer
{
    class OrderGroup;
    class Game;
    class Path;
    class PathTile;
    class Tribe;
    class Inventory;

    class Unit
    {

    public:

        Unit(Game *game, int id, Tribe *tribe, int x, int y, UnitTemplate *unitTemplate);
        ~Unit();
        void update();
        void startPath();
        void setOrderGroup(std::shared_ptr<OrderGroup> group);
        void sendUpdate(PlayerData *playerData, bool overrideDirty = false);
        bool anyVarIsDirty(PlayerData *playerData);
        void cleanAllVars();
        bool shouldPushOtherUnit(Unit *otherUnit, bool inPathfinding);
        bool withinRangeOfUnit(int x, int y, int range, Unit *targetUnit);
        bool isVisibleToTribe(Tribe *tribe);
        void stop(std::shared_ptr<OrderGroup> orderGroup);
        void hold();
        bool canMove();
        bool canAttack();
        void trainUnit(UnitTemplate *unitTemplate);
        void finishTraining(UnitTemplate *unitTemplate);

        int id = -1;
        Synced<AnimationState>* animationState = nullptr;
        Tribe *tribe = nullptr;
        UnitTemplate *unitTemplate = nullptr;

        //pathfinding vars
        std::shared_ptr<OrderGroup> orderGroup = nullptr;
        PathTile *lastKnownLongPathTile = nullptr;
        int lostWithoutShortPath = 0;
        int pushDirection = 0;
        int disToEnd = 0;
        bool followingPath = false;
        int walkSpeed = 0;
        int walkAmount = 0;
        Point* nextTilePosition = nullptr;
        std::shared_ptr<DsprGameServer::Path> shortPath = nullptr;
        PathTile* shortPathCurrentTile = nullptr;
        Point* position = nullptr;
        Synced<Point>* nextPosition = nullptr;
        Synced<Point>* moveTarget = nullptr;

        //attacking vars
        Synced<Int>* health = nullptr;
        Synced<Int>* syncedTargetUnitId = nullptr;
        int stamina = 100;

        const int minDamage = 10;
        const int maxDamage = 15;
        int range = 2;//make sure this is multiples of 2... to accomodate for diagonal tiles
        float attackFrameIndex = 0;
        float attackWaitIndex = 0;
        float attackAnimationSpeed = 1;
        float attackWaitSpeed = 1.0f / 1.7f;
        const int attackFramesNumber = 5;
        const int attackFrameToApplyDamage = 3;
        const int attackWaitFrames = 5;

        int gatherFrameIndex = 0;
        const int gatherFrameToReceiveResource = 110;
        Synced<Point>* gatherYield = nullptr;
        SpecificUnit* specificUnit = nullptr;

        Inventory* inventory = nullptr;
    private:

        Game* game = nullptr;
        int timesHaventPushed = 0;
        int pushCount = 0;
        std::set<Unit*>* blockedEnemyList = nullptr;
        ConstructionQueue* constructionQueue = nullptr;

        void pushOtherUnit(Unit *otherUnit);
        void getNextTile();
        int getDir(int x, int y);
        Point *getPointFromDir(int dir);
        void setPathArrived();
        Unit *getUnitAtPosition(int x, int y);
        void getNextTileSimplePathfind();
        void setPathUnarrived();
        void updateNextPosition(Point *newNextPosition);
        Unit *getEnemyUnitInRange(int range);
        void updateAttackMoving();
        void lookForEnemyUnitsAndEngage();
        void addToBlockedEnemyList(Unit *blockedEnemy);
        void damageOtherUnit(Unit *otherUnit, int dmgAmount);

        void updateStanding();
        void updateWalking();
        void updateFollowing();
        void updateAttacking();
        void updateHolding();
        void updateGather();

        void handleAttackAnimation(Unit *targetUnit);
        void handleGatherAnimation(Unit *targetUnit);

        void setAnimationStateHeading(Unit *targetUnit);

        int getAdjustedDir(float x, float y);

        void updatePickup();

        bool withinRangeOfPoint(int x, int y, int range, int tx, int ty);

        void pickupItem(DsprGameServer::Item *item);


        void updateItemDrop();

        void dropItem();

        void updateItemGive();
    };
}