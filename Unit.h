#pragma once

//
// Created by connor on 8/1/18.
//

#include "Point.h"
#include "Synced.h"
#include "Player.h"
#include "Pathfinding/Path.h"

namespace DsprGameServer
{
    class Unit
    {
    public:
        Unit(int id, int x, int y);
        ~Unit();
        int id = -1;
        Point* position = nullptr;
        Synced<Point>* nextPosition = nullptr;
        Point* moveTarget = nullptr;

        const int walkSpeedStraight = 500;
        const int walkSpeedDiagonal = (int) (walkSpeedStraight / 1.4f);
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
    };
}