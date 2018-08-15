//
// Created by connor on 8/1/18.
//

#include <sstream>
#include "Unit.h"
#include "MathUtils.h"
#include "GameServer.h"
#include "Pathfinding/PathTile.h"
#include "UnitManager.h"

namespace DsprGameServer
{
    Unit::Unit(Game *game, int id, int x, int y)
    {
        this->game = game;
        this->id = id;
        this->position = new Point(x, y);
        this->nextPosition = new Synced<Point>("nextPosition", new Point(x, y));
        this->moveTarget = new Synced<Point>("moveTarget", new Point(x, y));
    }

    Unit::~Unit()
    {
        delete this->position;
        delete this->nextPosition;
        delete this->moveTarget;
    }

    void Unit::update()
    {
        if (!this->position->Equals(this->nextPosition->obj()))
        {
            this->walkAmount += this->walkSpeed;
            if (this->walkAmount >= this->walkMax)
            {
                walkAmount = 0;
                this->position->Set(this->nextPosition->obj());
            }
        }

        if (this->walkAmount == 0 && this->followingPath)
        {
            if (this->currentPathTile != nullptr)
            {
                PathTile* nextTile = this->currentPathTile->nextTile;
                if (nextTile == nullptr)
                {
                    //maybe this tile is end!
                    if (this->position->Equals(this->moveTarget->obj()))
                    {
                        this->followingPath = false;
                    }
                }
                else
                {
                    auto unitAtNextPosition = this->game->unitManager->getUnitWithNextPosition(nextTile->x, nextTile->y);
                    if (unitAtNextPosition == nullptr)
                    {
                        currentPathTile = nextTile;
                        int difx = currentPathTile->x - this->position->x;
                        int dify = currentPathTile->y - this->position->y;
                        if (difx == 2 || dify == 2 || difx == -2 || dify == -2) {
                            walkSpeed = walkSpeedDiagonal;
                        } else {
                            walkSpeed = walkSpeedStraight;
                        }

                        this->nextPosition->dirtyObj()->Set(currentPathTile->x, currentPathTile->y);
                    }
                }
            }
            else
            {
                this->followingPath = false;
            }
        }
    }

    void Unit::sendUpdate(DsprGameServer::Player* player)
    {
        if (!this->anyVarIsDirty()) return;
        std::stringstream msg;
        msg << "unit/1.0/update|" << id << "|";
        bool firstVar = true;

        if (this->nextPosition->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->nextPosition->serialize();
        }

        if (this->moveTarget->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->moveTarget->serialize();
        }

        //next synced variable should follow this format
//        if (this->nextPosition->isDirty())
//        {
//            msg << "&" << this->nextPosition->serialize();
//        }

        msg << "\r\n";
        GameServer::get().queueMessage(player->getWs(), msg.str());
    }

    bool Unit::anyVarIsDirty()
    {
        if (this->nextPosition->isDirty()) return true;
        if (this->moveTarget->isDirty()) return true;
        //more synced vars here
        return false;
    }

    void Unit::cleanAllVars() {
        this->nextPosition->clean();
        this->moveTarget->clean();
        //more synced vars here
    }

    void Unit::startPath(std::shared_ptr<DsprGameServer::Path> path)
    {
        this->path = path;
        this->followingPath = true;
        this->currentPathTile = this->path->getTile(this->position->x, this->position->y);
        this->moveTarget->dirtyObj()->Set(this->path->targetX, this->path->targetY);
    }
}