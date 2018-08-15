#pragma once

//
// Created by connor on 8/1/18.
//

#include "Point.h"
#include "Synced.h"
#include "Player.h"
#include "Game.h"
//#include "Pathfinding/Path.h"

namespace DsprGameServer
{
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

        std::shared_ptr<Path> path = nullptr;
        bool followingPath = false;
        PathTile* currentPathTile = nullptr;
    private:
        Game* game = nullptr;
    };
}