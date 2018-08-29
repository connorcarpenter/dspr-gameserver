//
// Created by connor on 8/1/18.
//

#include <sstream>
#include "Unit.h"
#include "../Math/MathUtils.h"
#include "../GameServer.h"
#include "../Pathfinding/PathTile.h"
#include "UnitManager.h"
#include "OrderGroup.h"

namespace DsprGameServer
{
    Unit::Unit(Game *game, int id, Tribe* tribe, int x, int y)
    {
        this->game = game;
        this->id = id;
        this->position = new Point(x, y);
        this->nextPosition = new Synced<Point>("nextPosition", new Point(x, y));
        this->moveTarget = new Synced<Point>("moveTarget", new Point(x, y));
        this->moveVector = new FloatPoint(0,0);
        this->tribe = tribe;
    }

    Unit::~Unit()
    {
        delete this->position;
        delete this->nextPosition;
        delete this->moveTarget;
        delete this->moveVector;
    }

    void Unit::update() {

        if (!this->position->Equals(this->nextPosition->obj()))
        {
            this->walkAmount += this->walkSpeed;
            if (this->walkAmount >= this->walkMax)
            {
                walkAmount = 0;
                this->position->Set(this->nextPosition->obj());
            }
        }

        if (this->walkAmount == 0 && this->orderGroup != nullptr) {
            switch (this->orderGroup->orderIndex) {
                case Move:
                    this->updateStandingWalking();
                    break;
                case Follow:
                    this->updateFollowing();
                    break;
                case AttackTarget:
                    this->updateAttacking();
                    break;
            }
        }

        if (this->moveVector->x != 0 || this->moveVector->y != 0)
        {
            auto movePoint = this->moveVector->ToMovePoint();

            if (movePoint->x == 0 || movePoint->y == 0)
            {
                movePoint->x *= 2;
                movePoint->y *= 2;
            }
            auto tileAtPosition = this->game->tileManager->getTileAt(this->position->x + movePoint->x, this->position->y + movePoint->y);
            if (tileAtPosition == nullptr || !tileAtPosition->walkable)
            {
                this->moveVector->Set(0,0);
            }
            else
            {
                auto unitAtNextPosition = this->game->unitManager->getUnitWithNextPosition(this->position->x + movePoint->x, this->position->y + movePoint->y);
                if (unitAtNextPosition == nullptr)
                {
                    int difx = movePoint->x;
                    int dify = movePoint->y;
                    if (difx == 2 || dify == 2 || difx == -2 || dify == -2)
                    {
                        walkSpeed = walkSpeedDiagonal;
                    }
                    else
                    {
                        walkSpeed = walkSpeedStraight;
                    }

                    this->nextPosition->dirtyObj()->Set(this->position->x + movePoint->x,
                                                        this->position->y + movePoint->y);
                    this->moveVector->Set(0, 0);
                }
                else
                {
                    if (!unitAtNextPosition->followingPath)
                    {
                        unitAtNextPosition->pushOtherUnit(unitAtNextPosition->position->x - this->position->x,
                                                          unitAtNextPosition->position->y - this->position->y, 1.0f);
                    }
                    else
                    {
                        if (this->orderGroup != nullptr) {
                            nextPathTile = getNextPathTile(this->currentPathTile, this->orderGroup->path);
                        }
                    }
                }
            }
            delete movePoint;
        }
    }

    void Unit::updateStandingWalking() {
        if (this->followingPath) {
            if (this->currentPathTile != nullptr) {
                if (this->position->x == nextPathTile->x && this->position->y == nextPathTile->y) {
                    currentPathTile = nextPathTile;
                    nextPathTile = getNextPathTile(this->currentPathTile, this->orderGroup->path);
                    if (nextPathTile != nullptr)
                        this->direction = getDir(this->nextPathTile->x - this->currentPathTile->x,
                                                 this->nextPathTile->y - this->currentPathTile->y);
                }

                if (nextPathTile == nullptr || (currentPathTile->heat <= this->orderGroup->getAcceptableHeat())) {
                    this->moveVector->Set(0, 0);
                    this->followingPath = false;
                    this->orderGroup->unitArrived();
                } else {
                    this->moveVector->Add(nextPathTile->x - this->position->x, nextPathTile->y - this->position->y);
                }
            } else {
                this->followingPath = false;
            }
        }
    }

    void Unit::updateFollowing()
    {
        if (this->followingPath) {
            if (this->currentPathTile != nullptr) {
                if (this->position->x == nextPathTile->x && this->position->y == nextPathTile->y) {
                    currentPathTile = nextPathTile;
                    nextPathTile = getNextPathTile(this->currentPathTile, this->orderGroup->path);
                    if (nextPathTile != nullptr)
                        this->direction = getDir(this->nextPathTile->x - this->currentPathTile->x,
                                                 this->nextPathTile->y - this->currentPathTile->y);

                        this->orderGroup->recalculatePathIfTargetMoved();
                }

                if (nextPathTile == nullptr || (currentPathTile->heat <= this->orderGroup->getAcceptableHeat())) {
                    this->moveVector->Set(0, 0);
                    this->followingPath = false;
                    this->orderGroup->unitArrived();
                } else {
                    this->moveVector->Add(nextPathTile->x - this->position->x, nextPathTile->y - this->position->y);
                }
            } else {
                this->followingPath = false;
            }
        } else{
            this->orderGroup->recalculatePathIfTargetMoved();
        }
    }

    void Unit::updateAttacking() {
        if (this->followingPath) {
            if (this->currentPathTile != nullptr) {
                if (this->position->x == nextPathTile->x && this->position->y == nextPathTile->y) {
                    currentPathTile = nextPathTile;
                    nextPathTile = getNextPathTile(this->currentPathTile, this->orderGroup->path);
                    if (nextPathTile != nullptr)
                        this->direction = getDir(this->nextPathTile->x - this->currentPathTile->x,
                                                 this->nextPathTile->y - this->currentPathTile->y);

                        this->orderGroup->recalculatePathIfTargetMoved();
                }

                if (nextPathTile == nullptr || (currentPathTile->heat <= this->orderGroup->getAcceptableHeat())) {
                    this->moveVector->Set(0, 0);
                    this->followingPath = false;
                    this->orderGroup->unitArrived();
                } else {
                    this->moveVector->Add(nextPathTile->x - this->position->x, nextPathTile->y - this->position->y);
                }
            } else {
                this->followingPath = false;
            }
        }
    }

    void Unit::startPath()
    {
        auto path = this->orderGroup->path;
        if (this->walkAmount > 0)
        {
            this->currentPathTile = path->getTile(this->nextPosition->obj()->x, this->nextPosition->obj()->y);
        }
        else
        {
            this->currentPathTile = path->getTile(this->position->x, this->position->y);
        }
        if (this->currentPathTile != nullptr)
        {
            this->followingPath = true;
            this->nextPathTile = this->currentPathTile->nextTile;
            this->direction = getDir(this->nextPathTile->x - this->currentPathTile->x,
                                     this->nextPathTile->y - this->currentPathTile->y);
            this->nextPathTile = getNextPathTile(this->currentPathTile, path);
            this->moveTarget->dirtyObj()->Set(path->targetX, path->targetY);
        }
    }

    void Unit::pushOtherUnit(int x, int y, float mag)
    {
        auto ran = MathUtils::getRandom(3);
        switch (ran)
        {
            case 0:
                {
                    auto tempDir = getDir(x, y);
                    tempDir += 1; if (tempDir>7)tempDir-=8;
                    Point* adj1 = getPointFromDir(tempDir);
                    this->moveVector->Add(mag*adj1->x, mag*adj1->y);
                    delete adj1;
                }
                return;
            case 1:
                {
                    auto tempDir = getDir(x, y);
                    tempDir -= 1; if (tempDir<0)tempDir+=8;
                    Point* adj1 = getPointFromDir(tempDir);
                    this->moveVector->Add(mag*adj1->x, mag*adj1->y);
                    delete adj1;
                }
                return;
            default:
                this->moveVector->Add(mag*x, mag*y);
                return;
        }
    }

    PathTile * Unit::getNextPathTile(PathTile *curTile, std::shared_ptr<DsprGameServer::Path> path)
    {
        PathTile* nextTile = nullptr;

        if (direction == -1)
        {
            if (curTile == nullptr) return nullptr;
            if (curTile->nextTile == nullptr) return nullptr;
            nextTile = nextTileIfFree(curTile->nextTile); if (nextTile != nullptr) return nextTile;
            direction = 0;
        }

        for(int i = 0; i<2;i++)
        {
            auto newDir = direction + i;
            if (newDir>7)newDir-=8;

            Point* adj1 = getPointFromDir(newDir);
            nextTile = nextTileCheck(path, curTile, adj1->x, adj1->y); if (nextTile != nullptr) return nextTile;
            delete adj1;

            if (i != 0)
            {
                newDir = direction - i;
                if (newDir<0)newDir+=8;
                Point* adj2 = getPointFromDir(newDir);
                nextTile = nextTileCheck(path, curTile, adj2->x, adj2->y); if (nextTile != nullptr) return nextTile;
                delete adj2;
            }
        }
        return curTile->nextTile;
    }

    PathTile *Unit::nextTileIfFree(PathTile *nextTile)
    {
        if (nextTile == nullptr) return nullptr;

        auto unitAtNextPosition = this->game->unitManager->getUnitWithNextPosition(nextTile->x, nextTile->y);
        if (unitAtNextPosition != nullptr) return nullptr;

        return nextTile;
    }

    PathTile *Unit::nextTileCheck(std::shared_ptr<DsprGameServer::Path> path, PathTile *curTile, int xAdj, int yAdj)
    {
        auto otherTile = path->getTile(curTile->x+xAdj, curTile->y+yAdj);
        if (otherTile == nullptr) return nullptr;
        if (otherTile->heat >= curTile->heat) return nullptr;
        return nextTileIfFree(otherTile);
    }

    void Unit::setOrderGroup(std::shared_ptr<OrderGroup> group)
    {
        if (this->orderGroup != nullptr)
        {
            this->orderGroup->removeUnit(this);
        }

        this->orderGroup = group;
        this->orderGroup->addUnit(this);
    }

    int Unit::getDir(int x, int y)
    {
        auto dir = (std::atan2(y,x)*180/M_PI);
        if (dir <= 0)
            dir = -dir;
        else
            dir = 360-dir;
        return (int) (dir/45);
    }

    Point *Unit::getPointFromDir(int dir)
    {
        switch(dir)
        {
            case 1:
                return new Point(1, -1);
            case 2:
                return new Point(0, -2);
            case 3:
                return new Point(-1, -1);
            case 4:
                return new Point(-2, 0);
            case 5:
                return new Point(-1, 1);
            case 6:
                return new Point(0, 2);
            case 7:
                return new Point(1, 1);
            default:
                return new Point(2, 0);
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
}