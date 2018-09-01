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
#include "../Pathfinding/SimplePathfinder.h"

namespace DsprGameServer
{
    Unit::Unit(Game *game, int id, Tribe* tribe, int x, int y)
    {
        this->game = game;
        this->id = id;
        this->position = new Point(x, y);
        this->nextPosition = new Synced<Point>("nextPosition", new Point(x, y));
        this->moveTarget = new Synced<Point>("moveTarget", new Point(x, y));
        this->animationState = new Synced<AnimationState>("animationState", new AnimationState());
        this->nextTilePosition = new Point(x,y);
        this->tribe = tribe;
    }

    Unit::~Unit()
    {
        delete this->position;
        delete this->nextPosition;
        delete this->moveTarget;
        delete this->nextTilePosition;
        delete this->animationState;
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

        if (!this->nextPosition->obj()->Equals(this->nextTilePosition))
        {
            int difx = this->nextTilePosition->x - this->position->x;
            int dify = this->nextTilePosition->y - this->position->y;

            if (difx == 0 || dify == 0)
            {
                this->walkSpeed = this->walkSpeedDiagonal;
            }
            else
            {
                this->walkSpeed = this->walkSpeedStraight;
            }

            this->nextPosition->dirtyObj()->Set(this->nextTilePosition);
        }
    }

    void Unit::updateStandingWalking() {
        if (this->followingPath) {
            if (this->position->Equals(this->nextTilePosition) && this->nextPosition->obj()->Equals(this->nextTilePosition)) {
                if (MathUtils::Distance(this->position->x, this->position->y, this->moveTarget->obj()->x, this->moveTarget->obj()->y)
                    <= (this->orderGroup->getAcceptableTilesToEnd()))
                {
                    setPathArrived();
                }
                else
                {
                    getNextTile();
                }
            }
        }
    }

    void Unit::startPath()
    {
        this->followingPath = true;
        this->moveTarget->dirtyObj()->Set(this->orderGroup->path->targetX, this->orderGroup->path->targetY);
        this->disToEnd = INT_MAX;
    }

    void Unit::setPathArrived()
    {
        this->followingPath = false;
        this->orderGroup->unitArrived();
    }

    void Unit::getNextTile()
    {
        bool shortPathSuccess = false;
        if (this->shortPath != nullptr)
        {
                if (this->shortPathCurrentTile != nullptr)
                {
                    auto nextTile = this->shortPathCurrentTile->nextTile;
                    if (nextTile != nullptr){
                        auto unitOnTile = getUnitAtPosition(nextTile->x, nextTile->y);
                        bool tileIsOccupied = unitOnTile != nullptr;
                        if (tileIsOccupied)
                        {
                            this->shortPath = nullptr;
                            this->shortPathCurrentTile = nullptr;
                        }
                        else
                        {
                            this->nextTilePosition->Set(nextTile->x, nextTile->y);
                            shortPathSuccess = true;
                        }
                    }
                }
        }

        if (shortPathSuccess)return;
        auto currentTileIfOnPath = this->orderGroup->path->getTile(this->position->x, this->position->y);
        bool onLongPath = currentTileIfOnPath != nullptr;
        if (onLongPath)
        {
            this->disToEnd = currentTileIfOnPath->disToEnd;
            auto nextTile = currentTileIfOnPath->nextTile;
            if(nextTile == nullptr)
            {
                this->disToEnd = 0;
                return;
            }
            else
            {
                auto unitOnTile = getUnitAtPosition(nextTile->x, nextTile->y);
                bool tileIsOccupied = unitOnTile != nullptr;
                if (tileIsOccupied)
                {
                    if (unitOnTile->followingPath)
                    {
                        getNextTileSimplePathfind();
                        return;
                    }
                    else
                    {
                        //pushOtherUnit(unitOnTile);
                        getNextTileSimplePathfind();
                        return;
                    }
                }
                else
                {
                    this->nextTilePosition->Set(nextTile->x, nextTile->y);
                    return;
                }
            }
        }
        else
        {
            getNextTileSimplePathfind();
        }
    }

    PathTile *Unit::nextTileCheck(std::shared_ptr<DsprGameServer::Path> path, PathTile *curTile, int xAdj, int yAdj)
    {
        auto otherTile = path->getTile(curTile->x+xAdj, curTile->y+yAdj);
        if (otherTile == nullptr) return nullptr;
        if (otherTile->disToEnd >= curTile->disToEnd) return nullptr;
        return nextTileIfFree(otherTile);
    }

    PathTile *Unit::nextTileIfFree(PathTile *nextTile)
    {
        if (nextTile == nullptr) return nullptr;

        auto unitAtNextPosition = this->game->unitManager->getUnitWithNextPosition(nextTile->x, nextTile->y);
        if (unitAtNextPosition != nullptr) return nullptr;

        return nextTile;
    }



    void Unit::setOrderGroup(std::shared_ptr<OrderGroup> group)
    {
        if (this->animationState->obj()->GetState() != Walking)
            this->animationState->dirtyObj()->SetState(Walking);

        if (this->orderGroup != nullptr)
        {
            this->orderGroup->removeUnit(this);
        }

        this->orderGroup = group;
        this->orderGroup->addUnit(this);
    }

    void Unit::pushOtherUnit(Unit *otherUnit)
    {
        ///TODO: make this work!
        //otherUnit->followingPath = true;
        //otherUnit->nextTilePosition
    }

    Unit *Unit::getUnitAtPosition(int x, int y) {
        return this->game->unitManager->getUnitWithNextPosition(x,y);
    }

    void Unit::getNextTileSimplePathfind() {
        Point* nextPoint = this->game->simplePathfinder->findNextPosition(this, this->orderGroup->path);
        if (nextPoint == nullptr) return;
        this->nextTilePosition->Set(nextPoint);
        delete nextPoint;
    }

    void Unit::updateFollowing()
    {
//        if (this->followingPath) {
//            if (this->currentPathTile != nullptr) {
//                if (this->position->x == nextPathTile->x && this->position->y == nextPathTile->y) {
//                    currentPathTile = nextPathTile;
//                    nextPathTile = getNextPathTile();
//                    this->orderGroup->recalculatePathIfTargetMoved();
//                }
//
//                if (nextPathTile == nullptr || (currentPathTile->heat <= this->orderGroup->getAcceptableHeat())) {
//                    this->moveVector->Set(0, 0);
//                    this->followingPath = false;
//                    this->orderGroup->unitArrived();
//                } else {
//                    this->moveVector->Add(nextPathTile->x - this->position->x, nextPathTile->y - this->position->y);
//                }
//            } else {
//                this->followingPath = false;
//            }
//        } else{
//            this->orderGroup->recalculatePathIfTargetMoved();
//        }
    }

    void Unit::updateAttacking() {

//        auto targetUnit = this->orderGroup->targetUnit;
//        int distanceToTarget = (int) MathUtils::Ceiling(MathUtils::Distance(this->position->x, this->position->y,
//                                                                            targetUnit->position->x, targetUnit->position->y));
//
//        if (distanceToTarget <= this->range){
//            //start to attack!
//            if (this->animationState->obj()->GetState() != Attacking) {
//                this->animationState->dirtyObj()->SetState(Attacking);
//                int newHeading = getDir(targetUnit->position->x - this->position->x, targetUnit->position->y - this->position->y);
//                this->animationState->dirtyObj()->SetHeading(newHeading);
//                this->attackFrameIndex = 0;
//            }
//
//            this->attackFrameIndex += 1;
//            if (this->attackFrameIndex == this->attackFrameToApplyDamage){
//                targetUnit->health -= this->damage;
//            }
//            if (this->attackFrameIndex >= this->attackFramesNumber){
//                this->attackFrameIndex = 0;
//            }
//        }
//        else {
//            //try to get in range of target unit
//            if (this->animationState->obj()->GetState() != Walking)
//                this->animationState->dirtyObj()->SetState(Walking);
//
//            if (this->followingPath) {
//                if (this->currentPathTile != nullptr) {
//                    if (this->position->x == nextPathTile->x && this->position->y == nextPathTile->y) {
//                        currentPathTile = nextPathTile;
//                        nextPathTile = getNextPathTile();
//
//                        this->orderGroup->recalculatePathIfTargetMoved();
//                    }
//
//                    if (nextPathTile == nullptr) {
//                        this->moveVector->Set(0, 0);
//                        this->followingPath = false;
//                        this->orderGroup->unitArrived();
//                    } else {
//                        this->moveVector->Add(nextPathTile->x - this->position->x, nextPathTile->y - this->position->y);
//                    }
//                } else {
//                    this->followingPath = false;
//                }
//            } else {
//                this->orderGroup->recalculatePathIfTargetMoved();
//            }
//        }
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

        if (this->animationState->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->animationState->serialize();
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
        if (this->animationState->isDirty()) return true;
        //more synced vars here
        return false;
    }

    void Unit::cleanAllVars() {
        this->nextPosition->clean();
        this->moveTarget->clean();
        this->animationState->clean();
        //more synced vars here
    }
}