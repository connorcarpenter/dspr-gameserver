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
#include "CircleCache.h"

namespace DsprGameServer
{
    Unit::Unit(Game *game, int id, Tribe* tribe, int x, int y)
    {
        this->game = game;
        this->id = id;
        this->position = new Point(x, y);
        this->nextPosition = new Synced<Point>("nextPosition", new Point(x, y));
        this->game->unitManager->addUnitToGrid(this);
        this->moveTarget = new Synced<Point>("moveTarget", new Point(x, y));
        this->animationState = new Synced<AnimationState>("animationState", new AnimationState());
        this->nextTilePosition = new Point(x,y);
        this->health = new Synced<Int>("health", new Int(100));
        this->tribe = tribe;
    }

    Unit::~Unit()
    {
        delete this->position;
        delete this->nextPosition;
        delete this->moveTarget;
        delete this->nextTilePosition;
        delete this->animationState;
        delete this->health;
        if (this->blockedEnemyList != nullptr) delete this->blockedEnemyList;
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

        if (this->walkAmount == 0) {
            if(this->orderGroup != nullptr) {
                switch (this->orderGroup->orderIndex) {
                    case Move:
                        this->updateWalking();
                        break;
                    case AttackMove:
                        this->updateAttackMoving();
                        break;
                    case Follow:
                        this->updateFollowing();
                        break;
                    case AttackTarget:
                        this->updateAttacking();
                        break;
                }
            } else {
                this->updateWalking();
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

            this->updateNextPosition(this->nextTilePosition);
        }
        else
        {
            if(!this->followingPath && (this->orderGroup==nullptr || (this->orderGroup != nullptr && this->orderGroup->orderIndex == Move)))
            {
                this->updateStanding();
            }
        }
    }

    void Unit::updateStanding() {
        this->lookForEnemyUnitsAndEngage();
    }

    void Unit::updateWalking() {
        if (this->followingPath && this->pushCount<5) {
            if (this->position->Equals(this->nextTilePosition) && this->nextPosition->obj()->Equals(this->nextTilePosition)) {
                if (this->orderGroup != nullptr) {
                    if (this->orderGroup->orderIndex == AttackTarget)
                    {
                        if (this->withinAttackRange(this->position->x, this->position->y, orderGroup->getTargetUnit())) {
                            setPathArrived();
                        }
                    }
                    else
                    {
                        if (this->orderGroup->path != nullptr) {
                            if (this->orderGroup->path->getEndTile(this->position->x, this->position->y) != nullptr) {
                                setPathArrived();
                            }
                        }
                    }
                    if (this->followingPath)
                        getNextTile();
                }
            }
        }
        else
        {
            if (this->pushCount > 0) {
            if (this->position->Equals(this->nextTilePosition) && this->nextPosition->obj()->Equals(this->nextTilePosition)) {
                    auto nextPoint = getPointFromDir(this->pushDirection);
                    int nextX = this->position->x + nextPoint->x;
                    int nextY = this->position->y + nextPoint->y;

                    auto tile = this->game->tileManager->getTileAt(nextX, nextY);
                    if (tile != nullptr && tile->walkable) {

                        auto unitOnTile = getUnitAtPosition(nextX, nextY);
                        bool tileIsOccupied = unitOnTile != nullptr;
                        if (tileIsOccupied) {
                            pushOtherUnit(unitOnTile);
                            this->pushDirection+=1;
                            if (this->pushDirection>7)this->pushDirection-=8;
                        } else {
                            nextTilePosition->x = nextX;
                            nextTilePosition->y = nextY;
                            this->pushCount = 0;
                            setPathUnarrived();
                        }
                    } else {
                        this->pushDirection+=1;
                        if (this->pushDirection>7)this->pushDirection-=8;
                    }
                    delete nextPoint;
                }
            }
        }
    }

    void Unit::startPath()
    {
        this->followingPath = true;
        this->moveTarget->dirtyObj()->Set(this->orderGroup->path->targetX, this->orderGroup->path->targetY);
        auto currentTile = this->orderGroup->path->getTile(this->nextPosition->obj()->x, this->nextPosition->obj()->y);
        if (currentTile == nullptr){
            this->disToEnd = INT_MAX;
        }
        else {
            this->disToEnd = currentTile->disToEnd;
        }
        this->lastKnownLongPathTile = currentTile;

        if (this->blockedEnemyList != nullptr){
            delete this->blockedEnemyList;
            this->blockedEnemyList = nullptr;
        }
    }

    void Unit::setPathArrived()
    {
        this->followingPath = false;
        this->orderGroup->unitArrived();
    }

    void Unit::setPathUnarrived()
    {
        this->followingPath = true;
        if (this->orderGroup != nullptr)
            this->orderGroup->unitUnarrived();
    }

    void Unit::getNextTile()
    {
        //if at the front of the pack, maybe just skip this step
        if (this->orderGroup->getUnitsArrived() == 0 && this->orderGroup->getNumberUnits() > 1) {
            int minDisInGroup = 0;
            int maxDisInGroup = 0;
            this->orderGroup->getMinAndMaxDisInGroup(minDisInGroup, maxDisInGroup);
            int range = maxDisInGroup-minDisInGroup;
            if (this->disToEnd <= minDisInGroup + (range/4)) {
                int preferredSpacing = this->orderGroup->getNumberUnits()*5;

                if (MathUtils::getRandom(MathUtils::Max(preferredSpacing+100, range))<range)
                {
                    return;
                }
            }
        }

        //if we are already on a shortPath, just follow it
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

                            if (shouldPushOtherUnit(unitOnTile, false))
                            {
                                pushOtherUnit(unitOnTile);
                                shortPathSuccess = true;
                            }
                        }
                        else
                        {
                            this->shortPathCurrentTile = nextTile;
                            this->nextTilePosition->Set(nextTile->x, nextTile->y);
                            this->disToEnd = nextTile->disToEnd;
                            shortPathSuccess = true;
                        }
                    } else {
                        this->shortPath = nullptr;
                        this->shortPathCurrentTile = nullptr;
                    }
                }
        }

        if (shortPathSuccess)return;

        //follow the longPath
        if (this->orderGroup->path == nullptr) return;
        auto currentTileIfOnPath = this->orderGroup->path->getTile(this->position->x, this->position->y);
        bool onLongPath = currentTileIfOnPath != nullptr;
        if (onLongPath)
        {
            this->lastKnownLongPathTile = currentTileIfOnPath;
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
                    if (shouldPushOtherUnit(unitOnTile, false))
                    {
                        pushOtherUnit(unitOnTile);
                        //getNextTileSimplePathfind();
                        return;
                    }
                    else
                    {
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
        this->timesHaventPushed = 0;

        if (otherUnit->pushCount == 0) {
            otherUnit->pushDirection = getDir(otherUnit->position->x - this->position->x,
                                              otherUnit->position->y - this->position->y);
            otherUnit->pushDirection += MathUtils::getRandom(3) - 1;
            if (otherUnit->pushDirection > 7) otherUnit->pushDirection -= 8;
            if (otherUnit->pushDirection < 0) otherUnit->pushDirection += 8;
        }

        otherUnit->pushCount += 1;
    }

    Unit *Unit::getUnitAtPosition(int x, int y) {
        return this->game->unitManager->getUnitFromGrid(x,y);
    }

    void Unit::getNextTileSimplePathfind() {
        Point* nextPoint = this->game->simplePathfinder->findNextPosition(this, this->orderGroup->path);
        if (nextPoint == nullptr){
            this->lostWithoutShortPath += 1;
            if (lostWithoutShortPath > 5){
                if(this->orderGroup->orderIndex == AttackTarget)
                {
                    if (lostWithoutShortPath > 50)
                    {
                        if (this->blockedEnemyList != nullptr){
                            delete this->blockedEnemyList;
                            this->blockedEnemyList = nullptr;
                        }
                        lostWithoutShortPath = 5;
                    }
                    else
                    {
                        this->addToBlockedEnemyList(this->orderGroup->getTargetUnit());
                        this->lookForEnemyUnitsAndEngage();
                    }
                }
                else
                {
                    this->disToEnd += 10;
                }
            }

            return;
        }
        this->lostWithoutShortPath = 0;
        auto unitOnTile = getUnitAtPosition(nextPoint->x, nextPoint->y);
        if (unitOnTile == nullptr) {
            this->nextTilePosition->Set(nextPoint);
        } else {
            pushOtherUnit(unitOnTile);
        }
        delete nextPoint;
    }

    void Unit::updateFollowing()
    {
        this->updateWalking();
        this->orderGroup->recalculatePathIfTargetMoved();
    }

    void Unit::updateAttackMoving() {
        this->lookForEnemyUnitsAndEngage();
        this->updateWalking();
    }

    void Unit::updateAttacking()
    {
        auto targetUnit = this->orderGroup->getTargetUnit();

        if (targetUnit == nullptr){
            this->orderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::Move);
            this->followingPath = false;
            if (this->animationState->obj()->GetState() != Walking) {
                this->animationState->dirtyObj()->SetState(Walking);
            }
            return;
        }

        if (this->withinAttackRange(this->position->x, this->position->y, targetUnit)){
            //start to attack!
            if (this->animationState->obj()->GetState() != Attacking) {
                this->animationState->dirtyObj()->SetState(Attacking);
                int newHeading = getDir(targetUnit->position->x - this->position->x, targetUnit->position->y - this->position->y);
                this->animationState->dirtyObj()->SetHeading(newHeading);
                this->attackFrameIndex = 0;
                this->orderGroup->unitArrived();
            } else {
                if (this->orderGroup->targetHasMoved()){
                    int newHeading = getDir(targetUnit->position->x - this->position->x, targetUnit->position->y - this->position->y);
                    if (this->animationState->obj()->GetHeading() != newHeading)
                        this->animationState->dirtyObj()->SetHeading(newHeading);
                }
            }

            this->attackFrameIndex += 1;
            if (this->attackFrameIndex == this->attackFrameToApplyDamage){
                this->damageOtherUnit(targetUnit, this->damage);
            }
            if (this->attackFrameIndex >= this->attackFramesNumber){
                this->attackFrameIndex = 0;
            }
        }
        else {
            //try to get in range of target unit
            if (this->animationState->obj()->GetState() != Walking) {
                this->animationState->dirtyObj()->SetState(Walking);
                this->orderGroup->unitUnarrived();
            }

            if(!this->followingPath) setPathUnarrived();
            this->updateFollowing();
        }
    }

    bool Unit::shouldPushOtherUnit(Unit *otherUnit, bool inPathfinding) {
        if (otherUnit->tribe != this->tribe && !inPathfinding) return false;
        if (otherUnit->animationState->obj()->GetState() == Attacking) return false;
        if (inPathfinding)
            return (!otherUnit->followingPath || otherUnit->timesHaventPushed>20) && (otherUnit->orderGroup.get() != this->orderGroup.get() || (this->timesHaventPushed>20));
        bool shouldPush = (!otherUnit->followingPath || otherUnit->timesHaventPushed>20) && (otherUnit->orderGroup.get() != this->orderGroup.get() || (this->timesHaventPushed>20));
        if (!shouldPush) this->timesHaventPushed += 1;
        return shouldPush;
    }

    void Unit::updateNextPosition(Point *newNextPosition) {
        this->game->unitManager->removeUnitFromGrid(this);
        this->nextPosition->dirtyObj()->Set(newNextPosition);
        this->game->unitManager->addUnitToGrid(this);
    }

    Unit* Unit::getEnemyUnitInAcquisitionRange(){
        auto acquiCircle = CircleCache::get().getCircle(this->acquisition);

        for(auto circleCoord : acquiCircle->coordList){
            auto unitAtPosition = this->getUnitAtPosition(this->position->x + circleCoord->x, this->position->y + circleCoord->y);
            if (unitAtPosition == nullptr)continue;
            if (unitAtPosition->tribe == this->tribe)continue;
            if (this->blockedEnemyList != nullptr) if (this->blockedEnemyList->count(unitAtPosition) > 0) continue;
            return unitAtPosition;
        }

        return nullptr;
    }

    bool Unit::withinAttackRange(int x, int y, Unit *targetUnit) {
        if (targetUnit == nullptr)
            return false;
        int distanceToTarget = MathUtils::Ceiling(MathUtils::Distance(x, y, targetUnit->position->x, targetUnit->position->y));
        return (distanceToTarget <= this->range);
    }

    void Unit::lookForEnemyUnitsAndEngage() {
        Unit* enemyUnitInAcquisitionRange = this->getEnemyUnitInAcquisitionRange();
        if (enemyUnitInAcquisitionRange != nullptr){
            //ideally, we would store the current order in a stack, and just push it down.. for now, no such thing
            std::list<std::pair<int, int>> unitPositionsList;

            auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::AttackTarget);
            newOrderGroup->setTargetUnit(enemyUnitInAcquisitionRange);

            unitPositionsList.emplace_back(std::pair<int,int>(this->position->x, this->position->y));
            this->setOrderGroup(newOrderGroup);

            auto path = this->game->pathfinder->findPath(unitPositionsList, enemyUnitInAcquisitionRange->position->x,
                                                         enemyUnitInAcquisitionRange->position->y, false);
            if (path != nullptr)
            {
                newOrderGroup->setPath(path);
                this->startPath();
            }
        }
    }

    void Unit::addToBlockedEnemyList(Unit *blockedEnemy) {
        if (this->blockedEnemyList == nullptr){
            this->blockedEnemyList = new std::set<Unit*>();
        }
        this->blockedEnemyList->insert(blockedEnemy);
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

    void Unit::sendUpdate(PlayerData *playerData)
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

        if (this->health->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->health->serialize();
        }

        //next synced variable should follow this format
//        if (this->nextPosition->isDirty())
//        {
//            msg << "&" << this->nextPosition->serialize();
//        }

        msg << "\r\n";
        GameServer::get().queueMessage(playerData, msg.str());
    }

    bool Unit::anyVarIsDirty()
    {
        if (this->nextPosition->isDirty()) return true;
        if (this->moveTarget->isDirty()) return true;
        if (this->animationState->isDirty()) return true;
        if (this->health->isDirty()) return true;
        //more synced vars here
        return false;
    }

    void Unit::cleanAllVars() {
        this->nextPosition->clean();
        this->moveTarget->clean();
        this->animationState->clean();
        this->health->clean();
        //more synced vars here
    }

    void Unit::damageOtherUnit(Unit *otherUnit, int dmgAmount) {
        otherUnit->health->dirtyObj()->Subtract(dmgAmount);
        if (otherUnit->health->obj()->value <= 0){
            this->game->unitManager->queueUnitForDeletion(otherUnit);
        }
    }
}