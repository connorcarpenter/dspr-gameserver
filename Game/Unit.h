#pragma once

//
// Created by connor on 8/1/18.
//

#include "../Math/Point.h"
#include "Synced.h"
#include "../Player.h"
#include "Game.h"
#include "../Math/FloatPoint.h"
#include "AnimationState.h"

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
        void updateStandingWalking();
        void updateFollowing();
        void updateAttacking();
        void startPath();
        void setOrderGroup(std::shared_ptr<OrderGroup> group);
        void sendUpdate(DsprGameServer::Player* player);
        bool anyVarIsDirty();
        void cleanAllVars();

        const int walkMax = 6;
        const int walkSpeedStraight = walkMax / 2; //2 updates to walk straight
        const int walkSpeedDiagonal = walkMax / 3; //3 updates to walk diagonal

        int id = -1;
        Point* position = nullptr;
        Synced<Point>* nextPosition = nullptr;
        Synced<Point>* moveTarget = nullptr;
        Synced<AnimationState>* animationState = nullptr;

        bool followingPath = false;
        PathTile* currentPathTile = nullptr;
        PathTile* nextPathTile = nullptr;
        int walkSpeed = 0;
        int walkAmount = 0;

        int health = 100;
        int stamina = 100;
        int damage = 1;
        int range = 2;//make sure this is multiples of 2... to accomodate for diagonal tiles
        int attackFrameIndex = 0;
        int attackFramesNumber = 5;
        int attackFrameToApplyDamage = 3;

        Tribe *tribe = nullptr;

    private:

        Game* game = nullptr;
        FloatPoint* moveVector = nullptr;
        std::shared_ptr<OrderGroup> orderGroup = nullptr;
        int direction = -1;

        void pushOtherUnit(int x, int y, float mag);

        PathTile *getNextPathTile(PathTile *curTile, std::shared_ptr<DsprGameServer::Path> path);

        PathTile *nextTileIfFree(PathTile *nextTile);

        PathTile *nextTileCheck(std::shared_ptr<DsprGameServer::Path> path, PathTile *curTile, int xAdj, int yAdj);

        int getDir(int x, int y);

        Point *getPointFromDir(int dir);
    };
}