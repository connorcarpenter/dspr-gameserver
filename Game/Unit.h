#pragma once

//
// Created by connor on 8/1/18.
//

#include <set>
#include "../Math/Point.h"
#include "Synced.h"
#include "Game.h"
#include "../Math/FloatPoint.h"
#include "AnimationState.h"
#include "../Math/Int.h"

namespace DsprGameServer
{
    class OrderGroup;
    class Game;
    class Path;
    class PathTile;
    class Tribe;

    enum UnitOrder { Move, AttackMove, Follow, AttackTarget };

    class Unit
    {

    public:

        Unit(Game *game, int id, Tribe *tribe, int x, int y);
        ~Unit();
        void update();
        void updateStanding();
        void updateWalking();
        void updateFollowing();
        void updateAttacking();
        void startPath();
        void setOrderGroup(std::shared_ptr<OrderGroup> group);
        void sendUpdate(PlayerData *playerData);
        bool anyVarIsDirty();
        void cleanAllVars();
        bool shouldPushOtherUnit(Unit *otherUnit, bool inPathfinding);
        bool withinAttackRange(int x, int y, Unit *targetUnit);

        const int walkMax = 6;
        const int walkSpeedStraight = walkMax / 2; //2 updates to walk straight
        const int walkSpeedDiagonal = walkMax / 3; //3 updates to walk diagonal

        int id = -1;
        Point* position = nullptr;
        Synced<Point>* nextPosition = nullptr;
        Synced<Point>* moveTarget = nullptr;
        Synced<AnimationState>* animationState = nullptr;
        Point* nextTilePosition = nullptr;
        std::shared_ptr<DsprGameServer::Path> shortPath = nullptr;
        PathTile* shortPathCurrentTile = nullptr;

        bool followingPath = false;
        int walkSpeed = 0;
        int walkAmount = 0;

        Synced<Int>* health = nullptr;
        int stamina = 100;
        int damage = 5;
        int range = 2;//make sure this is multiples of 2... to accomodate for diagonal tiles
        int attackFrameIndex = 0;
        int attackFramesNumber = 5;
        int attackFrameToApplyDamage = 3;
        const int sight = 8;
        const int acquisition = 8;

        Tribe *tribe = nullptr;

        int disToEnd = 0;

        std::shared_ptr<OrderGroup> orderGroup = nullptr;
        PathTile *lastKnownLongPathTile = nullptr;
        int lostWithoutShortPath = 0;
        int pushDirection = 0;

    private:

        Game* game = nullptr;
        int timesHaventPushed = 0;
        int pushCount = 0;
        std::set<Unit *> *blockedEnemyList = nullptr;

        void pushOtherUnit(Unit *otherUnit);
        void getNextTile();
        int getDir(int x, int y);
        Point *getPointFromDir(int dir);
        void setPathArrived();
        Unit *getUnitAtPosition(int x, int y);
        void getNextTileSimplePathfind();
        void setPathUnarrived();
        void updateNextPosition(Point *newNextPosition);
        Unit* getEnemyUnitInAcquisitionRange();
        void updateAttackMoving();
        void lookForEnemyUnitsAndEngage();
        void addToBlockedEnemyList(Unit *blockedEnemy);
        void damageOtherUnit(Unit *otherUnit, int dmgAmount);
    };
}