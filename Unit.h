#pragma once

//
// Created by connor on 8/1/18.
//

#include "Point.h"
#include "Synced.h"
#include "Player.h"
#include "Game.h"
#include "FloatPoint.h"

//#include "Pathfinding/Path.h"

namespace DsprGameServer
{
    class MoveGroup;
    class Game;
    class Path;
    class PathTile;

    class Unit
    {
    public:
        Unit(Game *game, int id, int x, int y);
        ~Unit();
        int id = -1;
        Point* position = nullptr;
        Synced<Point>* nextPosition = nullptr;
        Synced<Point>* moveTarget = nullptr;

        const int walkMax = 6;
        const int walkSpeedStraight = walkMax / 2;
        const int walkSpeedDiagonal = walkMax / 3;
        int walkSpeed = 0;
        int walkAmount = 0;


        void update();

        void sendUpdate(DsprGameServer::Player* player);

        bool anyVarIsDirty();

        void cleanAllVars();

        void startPath(std::shared_ptr<Path> path);
        void setMoveGroup(std::shared_ptr<MoveGroup> group);

        std::shared_ptr<Path> path = nullptr;
        bool followingPath = false;
        PathTile* currentPathTile = nullptr;
        PathTile* nextPathTile = nullptr;
    private:
        Game* game = nullptr;
        FloatPoint* moveVector = nullptr;
        std::shared_ptr<MoveGroup> moveGroup = nullptr;
        int direction = -1;

        void Push(int x, int y, float mag);

        PathTile *getNextPathTile(PathTile *curTile, std::shared_ptr<DsprGameServer::Path> path);

        bool losToNextTile(PathTile *nextTile);

        void addCohesion();

        PathTile *nextTileIfFree(PathTile *nextTile);

        PathTile *nextTileCheck(std::shared_ptr<DsprGameServer::Path> path, PathTile *curTile, int xAdj, int yAdj);

        int getDir(int x, int y);

        Point *getPointFromDir(int dir);
    };
}