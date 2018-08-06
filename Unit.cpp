//
// Created by connor on 8/1/18.
//

#include <sstream>
#include "Unit.h"
#include "MathUtils.h"
#include "GameServer.h"

namespace DsprGameServer
{
    Unit::Unit(int id, int x, int y)
    {
        this->id = id;
        this->position = new Point(x, y);
        this->nextPosition = new Synced<Point>("nextPosition", new Point(x, y));
        this->moveTarget = new Point(x, y);
    }

    Unit::~Unit()
    {
        delete this->position;
        delete this->nextPosition;
        delete this->moveTarget;
    }

    void Unit::update()
    {
        if (!this->position->Equals(this->nextPosition->get()))
        {
            this->walkAmount += this->walkSpeed;
            if (this->walkAmount >= 1000)
            {
                walkAmount = 0;
                this->position->Set(this->nextPosition->get());
            }
        }

        if (this->walkAmount == 0 && this->followingPath)
        {
            PathTile* nextTile = this->path->getPathTile(this->currentPathTile->getNextTile());
            if (nextTile == nullptr)
            {
                //maybe next tile is end!
                nextTile = this->path->getEndTile(this->currentPathTile->getNextTile());
                if (nextTile == nullptr) {
                    auto i = 1/0; //error!
                } else {
                    this->followingPath = false;
                }
            }
            currentPathTile = nextTile;
            int difx = currentPathTile->x - this->position->x;
            int dify = currentPathTile->y - this->position->y;
            if (difx == 2 || dify == 2)
            {
                walkSpeed = walkSpeedDiagonal;
            }
            else
            {
                walkSpeed = walkSpeedStraight;
            }

            this->nextPosition->get()->Set(this->position->x + difx, this->position->y + dify);
            this->nextPosition->dirty();
        }
    }

    void Unit::sendUpdate(DsprGameServer::Player* player)
    {
        if (!this->anyVarIsDirty()) return;
        std::stringstream msg;
        msg << "unit/1.0/update|" << id;

        if (this->nextPosition->isDirty())
        {
            msg << "|" << this->nextPosition->serialize();
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
        //more synced vars here
        return false;
    }

    void Unit::cleanAllVars() {
        this->nextPosition->clean();
    }

    void Unit::startPath(std::shared_ptr<DsprGameServer::Path> path)
    {
        this->path = path;
        this->followingPath = true;
        this->currentPathTile = this->path->getStartTile(this->position->x, this->position->y);

    }
}