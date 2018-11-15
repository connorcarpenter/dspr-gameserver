//
// Created by connor on 8/1/18.
//

#include <sstream>
#include "Unit.h"
#include "../../Math/MathUtils.h"
#include "../../GameServer.h"
#include "../../Pathfinding/PathTile.h"
#include "UnitManager.h"
#include "OrderGroup.h"
#include "../../Pathfinding/SimplePathfinder.h"
#include "../Circle/CircleCache.h"
#include "../FogManager.h"
#include "UnitTemplate.h"
#include "../IsoBox/IsoBoxCache.h"
#include "../Tribe.h"
#include "../EconomyManager.h"
#include "SpecificUnit/Manafount.h"
#include "../Item/Inventory.h"
#include "../Item/ItemManager.h"

namespace DsprGameServer
{
    Unit::Unit(Game *game, int id, Tribe *tribe, int x, int y, UnitTemplate *unitTemplate)
    {
        this->game = game;
        this->id = id;
        this->position = new Point(x, y);
        this->nextPosition = new Synced<Point>("nextPosition", new Point(x, y));
        this->moveTarget = new Synced<Point>("moveTarget", new Point(x, y));
        this->animationState = new Synced<AnimationState>("animationState", new AnimationState());
        this->gatherYield = new Synced<Point>("gatherYield", new Point(0,0));
        this->bleed = new Synced<Bool>("bleed", new Bool(false));
        this->bleed->setCleanFunc([&](){
            this->bleed->obj()->Set(false);
        });
        this->nextTilePosition = new Point(x,y);
        this->tribe = tribe;

        this->unitTemplate = unitTemplate;
        this->health = new Synced<Int>("health", new Int(unitTemplate->maxHealth));
        this->syncedTargetUnitId = new Synced<Int>("targetUnitId", new Int(-1));
        this->game->unitManager->addUnitToGrid(this);
        this->game->fogManager->revealFog(this->tribe, this->nextPosition->obj()->x, this->nextPosition->obj()->y, this->unitTemplate->sight);

        if (this->unitTemplate->hasConstructionQueue)
        {
            this->constructionQueue = new ConstructionQueue(this);
        }

        if (this->unitTemplate->hasInventory)
        {
            this->inventory = new Inventory(this);
        }

        if (this->unitTemplate->createSpecificUnitFunction != nullptr)
        {
            this->specificUnit = this->unitTemplate->createSpecificUnitFunction(this);
        }
    }

    Unit::~Unit()
    {
        this->game->fogManager->conceilFog(this->tribe, this->nextPosition->obj()->x, this->nextPosition->obj()->y, this->unitTemplate->sight);

        delete this->position;
        delete this->nextPosition;
        delete this->moveTarget;
        delete this->nextTilePosition;
        delete this->animationState;
        delete this->gatherYield;
        delete this->health;
        delete this->bleed;
        delete this->syncedTargetUnitId;
        if (this->blockedEnemyList != nullptr) delete this->blockedEnemyList;

        if (this->unitTemplate->hasConstructionQueue)
        {
            delete this->constructionQueue;
        }

        if (this->specificUnit != nullptr)
        {
            delete this->specificUnit;
        }
    }

    void Unit::update() {

        if (this->walkSpeed != 0) {
            if (!this->position->Equals(this->nextPosition->obj())) {
                this->walkAmount += this->walkSpeed;
                if (this->walkAmount >= this->unitTemplate->walkMax) {
                    walkAmount = 0;
                    this->position->Set(this->nextPosition->obj());
                }
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
                    case Hold:
                        this->updateHolding();
                        break;
                    case Gather:
                        this->updateGather();
                        break;
                    case Pickup:
                        this->updatePickup();
                        break;
                    case ItemDrop:
                        this->updateItemDrop();
                        break;
                    case ItemGive:
                        this->updateItemGive();
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
                this->walkSpeed = this->unitTemplate->walkSpeedDiagonal;
            }
            else
            {
                this->walkSpeed = this->unitTemplate->walkSpeedStraight;
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

        //construction queue
        if (this->constructionQueue != nullptr)
        {
            this->constructionQueue->update();
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
                        if (this->withinRangeOfUnit(this->position->x, this->position->y, this->range,
                                                    orderGroup->getTargetUnit())) {
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
                        if (tileIsOccupied && shouldPushOtherUnit(unitOnTile, false)) {
                            pushOtherUnit(unitOnTile);
                            this->pushDirection+=1;
                            if (this->pushDirection>7)this->pushDirection-=8;
                        } else {
                            if (!tileIsOccupied) {
                                if (!canMove()) {
                                    int i = 1/0; // how did we get here?
                                }
                                if (game->unitManager->getUnitFromGrid(nextX, nextY) != nullptr) {
                                    int i = 1/0; // how did we get here?
                                }
                                nextTilePosition->x = nextX;
                                nextTilePosition->y = nextY;
                                this->pushCount = 0;
                                setPathUnarrived();
                            } else {
                                this->pushDirection+=1;
                                if (this->pushDirection>7)this->pushDirection-=8;
                            }
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
            this->stop(std::make_shared<OrderGroup>(this->game, UnitOrder::Move));
            return;
        }

        if (this->withinRangeOfUnit(this->position->x, this->position->y, this->range, targetUnit)){
            //start to attack!
            this->handleAttackAnimation(targetUnit);
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

    void Unit::handleAttackAnimation(Unit* targetUnit)
    {
        if (this->animationState->obj()->GetState() != Attacking)
        {
            this->animationState->dirtyObj()->SetState(Attacking);
            setAnimationStateHeading(targetUnit);
            this->attackFrameIndex = 0;
            this->orderGroup->unitArrived();
        }
        else
        {
            if (this->orderGroup->targetHasMoved())
            {
                setAnimationStateHeading(targetUnit);
            }
        }

        if (this->attackWaitIndex <= 0)
        {
            this->attackFrameIndex += this->attackAnimationSpeed;
            if (this->attackFrameIndex == this->attackFrameToApplyDamage)
            {
                this->damageOtherUnit(targetUnit, MathUtils::getRandom(this->minDamage, this->maxDamage));
            }
            if (this->attackFrameIndex >= this->attackFramesNumber)
            {
                this->attackFrameIndex = 0;
                this->attackWaitIndex = this->attackWaitFrames;
            }
        }
        else
        {
            this->attackWaitIndex -= this->attackWaitSpeed;
        }
    }

    void Unit::updateGather()
    {
        auto targetUnit = this->orderGroup->getTargetUnit();

        if (targetUnit == nullptr){
            this->stop(std::make_shared<OrderGroup>(this->game, UnitOrder::Move));
            return;
        }

        if (this->withinRangeOfUnit(this->position->x, this->position->y, this->range, targetUnit)){
            //start to gather!
            this->handleGatherAnimation(targetUnit);
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

    void Unit::handleGatherAnimation(Unit* targetUnit)
    {
        if (this->animationState->obj()->GetState() != Gathering)
        {
            this->animationState->dirtyObj()->SetState(Gathering);
            setAnimationStateHeading(targetUnit);
            this->gatherFrameIndex = MathUtils::getRandom(-10,10);
            this->orderGroup->unitArrived();
            auto manafount = static_cast<Manafount*>(targetUnit->specificUnit);
            manafount->addGatheringUnit();

            auto animState = this->animationState->obj();
            animState->SetStateChangeFunction([animState, manafount](){
                manafount->removeGatheringUnit();
                animState->SetStateChangeFunction(nullptr);
            });
        }

        this->gatherFrameIndex += 1;

        if (this->gatherFrameIndex >= this->gatherFrameToReceiveResource)
        {
            auto curMana = this->game->economyManager->getMana(this->tribe);
            auto gatherRate = static_cast<Manafount*>(targetUnit->specificUnit)->gatherRate;
            curMana += gatherRate;
            this->game->economyManager->setManaClean(this->tribe, curMana);
            this->gatherYield->dirtyObj()->Set(gatherRate, curMana);
            this->gatherFrameIndex = MathUtils::getRandom(-10,10);
        }
    }

    void Unit::updatePickup()
    {
        auto targetItem = this->orderGroup->getTargetItem();

        if (targetItem == nullptr){
            this->stop(std::make_shared<OrderGroup>(this->game, UnitOrder::Move));
            return;
        }

        if (this->withinRangeOfPoint(this->position->x, this->position->y, 2, targetItem->position->x, targetItem->position->y)){
            this->pickupItem(targetItem);
        }
        else {
            //try to get in range of target unit
            if (this->animationState->obj()->GetState() != Walking) {
                this->animationState->dirtyObj()->SetState(Walking);
                this->orderGroup->unitUnarrived();
            }

            if(!this->followingPath) setPathUnarrived();
            this->updateWalking();
        }
    }

    void Unit::pickupItem(DsprGameServer::Item *item) {
        this->game->itemManager->removeItem(item);
        this->inventory->addItem(item);
    }

    void Unit::updateItemDrop()
    {
        if (this->withinRangeOfPoint(this->position->x, this->position->y, 2, this->moveTarget->obj()->x, this->moveTarget->obj()->y)){
            this->dropItem();
        }
        else {
            //try to get in range of target unit
            if (this->animationState->obj()->GetState() != Walking) {
                this->animationState->dirtyObj()->SetState(Walking);
                this->orderGroup->unitUnarrived();
            }

            if(!this->followingPath) setPathUnarrived();
            this->updateWalking();
        }
    }

    void Unit::dropItem() {
        if (this->game->itemManager->canPlaceItemAtPos(this->moveTarget->obj()->x, this->moveTarget->obj()->y))
        {
            this->game->itemManager->dropItemFromInventory(this->moveTarget->obj()->x, this->moveTarget->obj()->y, this->inventory->itemToDrop);
            this->inventory->removeItem(this->inventory->itemToDrop);
        }

        this->inventory->itemToDrop = nullptr;
        this->stop(std::make_shared<OrderGroup>(this->game, UnitOrder::Move));
    }

    void Unit::updateItemGive()
    {
        auto targetUnit = this->orderGroup->getTargetUnit();

        if (targetUnit == nullptr){
            this->stop(std::make_shared<OrderGroup>(this->game, UnitOrder::Move));
            return;
        }

        if (this->withinRangeOfUnit(this->position->x, this->position->y, 2, targetUnit)){
            targetUnit->inventory->addItem(this->inventory->itemToDrop);
            this->inventory->removeItem(this->inventory->itemToDrop);
            this->inventory->itemToDrop = nullptr;
            this->stop(std::make_shared<OrderGroup>(this->game, UnitOrder::Move));
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

    void Unit::updateHolding()
    {
        if (this->orderGroup->getTargetUnit() == nullptr)
        {
            if (this->unitTemplate->acquisition <= 0) return;
            Unit *enemyUnitInAcquisitionRange = this->getEnemyUnitInRange(this->range);
            if (enemyUnitInAcquisitionRange != nullptr)
            {
                this->orderGroup->setTargetUnit(enemyUnitInAcquisitionRange);
            }
        }
        auto targetUnit = this->orderGroup->getTargetUnit();
        if (targetUnit != nullptr)
        {
            if (this->withinRangeOfUnit(this->position->x, this->position->y, this->range, targetUnit))
            {
                //start to attack!
                this->handleAttackAnimation(targetUnit);
            }
            else
            {
                this->orderGroup->setTargetUnit(nullptr);
            }
        }
        if (this->orderGroup->getTargetUnit() == nullptr)
        {
            if (this->animationState->obj()->GetState() != Walking)
            {
                this->animationState->dirtyObj()->SetState(Walking);
            }
        }
    }

    void Unit::stop(std::shared_ptr<OrderGroup> orderGroup)
    {
        this->setOrderGroup(orderGroup);
        this->followingPath = false;
        if (!this->moveTarget->obj()->Equals(this->nextPosition->obj()))
            this->moveTarget->dirtyObj()->Set(this->nextPosition->obj()->x, this->nextPosition->obj()->y);
        if (this->animationState->obj()->GetState() != Walking) {
            this->animationState->dirtyObj()->SetState(Walking);
        }
    }

    void Unit::hold()
    {
        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrder::Hold);
        this->setOrderGroup(newOrderGroup);
        this->followingPath = false;
        if (!this->moveTarget->obj()->Equals(this->nextPosition->obj()))
            this->moveTarget->dirtyObj()->Set(this->nextPosition->obj()->x, this->nextPosition->obj()->y);
    }

    void Unit::startPath()
    {
        this->followingPath = true;
        this->moveTarget->dirtyObj()->Set(this->orderGroup->path->targetX, this->orderGroup->path->targetY);
        auto currentTile = this->orderGroup->path->getTile(this->nextPosition->obj()->x, this->nextPosition->obj()->y);
        if (currentTile == nullptr)
        {
            this->disToEnd = INT_MAX;
        }
        else
        {
            this->disToEnd = currentTile->disToEnd;
        }
        this->lastKnownLongPathTile = currentTile;

        if (this->blockedEnemyList != nullptr)
        {
            delete this->blockedEnemyList;
            this->blockedEnemyList = nullptr;
        }
    }

    void Unit::setPathArrived()
    {
        this->followingPath = false;
        this->moveTarget->dirtyObj()->Set(this->position->x, this->position->y);
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
                            if (!canMove())
                            {
                                int i = 1/0; // how did we get here?
                            }
                            if (game->unitManager->getUnitFromGrid(nextTile->x, nextTile->y) != nullptr)
                            {
                                int i = 1/0; // how did we get here?
                            }
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
                    if (!canMove())
                    {
                        int i = 1/0; // how did we get here?
                    }
                    if (game->unitManager->getUnitFromGrid(nextTile->x, nextTile->y) != nullptr)
                    {
                        int i = 1/0; // how did we get here?
                    }
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

        this->syncedTargetUnitId->dirtyObj()->Set(this->orderGroup->targetUnitId);
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
            if (!canMove())
            {
                int i = 1/0; // how did we get here?
            }
            if (game->unitManager->getUnitFromGrid(nextPoint->x, nextPoint->y) != nullptr)
            {
                int i = 1/0; // how did we get here?
            }
            this->nextTilePosition->Set(nextPoint);
        } else {
            if (shouldPushOtherUnit(unitOnTile, false))
            {
                pushOtherUnit(unitOnTile);
            }
            else {
                int i = 0;///FIX THIS!
            }

        }
        delete nextPoint;
    }

    bool Unit::shouldPushOtherUnit(Unit *otherUnit, bool inPathfinding) {
        if (otherUnit == nullptr) return false;
        if (otherUnit->tribe != this->tribe && !inPathfinding) return false;
        if (otherUnit->animationState->obj()->GetState() == Attacking) return false;
        if (!otherUnit->canMove()) return false;
        if (inPathfinding)
            return (!otherUnit->followingPath || otherUnit->timesHaventPushed>20) && (otherUnit->orderGroup.get() != this->orderGroup.get() || (this->timesHaventPushed>20));
        bool shouldPush = (!otherUnit->followingPath || otherUnit->timesHaventPushed>20) && (otherUnit->orderGroup.get() != this->orderGroup.get() || (this->timesHaventPushed>20));
        if (!shouldPush) this->timesHaventPushed += 1;
        return shouldPush;
    }

    void Unit::updateNextPosition(Point *newNextPosition)
    {
        this->game->unitManager->removeUnitFromGrid(this);
        this->game->fogManager->conceilFog(this->tribe, this->nextPosition->obj()->x, this->nextPosition->obj()->y, this->unitTemplate->sight);

        this->nextPosition->dirtyObj()->Set(newNextPosition);

        this->game->unitManager->addUnitToGrid(this);
        this->game->fogManager->revealFog(this->tribe, this->nextPosition->obj()->x, this->nextPosition->obj()->y, this->unitTemplate->sight);
    }

    Unit * Unit::getEnemyUnitInRange(int range) {
        auto acquiCircle = CircleCache::get().getCircle(range);

        for(auto circleCoord : acquiCircle->coordList)
        {
            auto unitAtPosition = this->getUnitAtPosition(this->position->x + circleCoord->x, this->position->y + circleCoord->y);
            if (unitAtPosition == nullptr)continue;
            if (unitAtPosition->tribe->isNeutral())continue;
            if (unitAtPosition->tribe == this->tribe)continue;
            if (this->blockedEnemyList != nullptr)
                if (this->blockedEnemyList->count(unitAtPosition) > 0)
                    continue;
            return unitAtPosition;
        }

        return nullptr;
    }

    bool Unit::withinRangeOfUnit(int x, int y, int range, Unit *targetUnit) {
        if (targetUnit == nullptr)
            return false;
        if (targetUnit->unitTemplate->tileWidth == 1 && targetUnit->unitTemplate->tileHeight == 1)
        {
            int distanceToTarget = MathUtils::Ceiling(MathUtils::Distance(x, y, targetUnit->position->x, targetUnit->position->y));
            return (distanceToTarget <= range);
        }
        else
        {
            auto targetIsoBoxBase = IsoBoxCache::get().getIsoBox(targetUnit->unitTemplate->tileWidth, targetUnit->unitTemplate->tileHeight);

            for(auto isoBoxCoord : targetIsoBoxBase->coordList)
            {
                int distanceToTarget = MathUtils::Ceiling(MathUtils::Distance(x, y,
                                                                              targetUnit->position->x + isoBoxCoord->x,
                                                                              targetUnit->position->y + isoBoxCoord->y));
                if(distanceToTarget <= range)
                    return true;
            }

            return false;
        }
    }

    bool Unit::withinRangeOfPoint(int x, int y, int range, int tx, int ty) {

            int distanceToTarget = MathUtils::Ceiling(MathUtils::Distance(x, y, tx, ty));
            return (distanceToTarget <= range);
    }

    void Unit::lookForEnemyUnitsAndEngage() {
        if (this->unitTemplate->acquisition <= 0) return;

        Unit* enemyUnitInAcquisitionRange = this->getEnemyUnitInRange(this->unitTemplate->acquisition);
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

    void Unit::damageOtherUnit(Unit *otherUnit, int dmgAmount) {
        if (otherUnit->unitTemplate->isInvincible)return;
        otherUnit->health->dirtyObj()->Subtract(dmgAmount);
        otherUnit->bleed->dirtyObj()->Set(true);
        if (otherUnit->health->obj()->value <= 0){
            this->game->unitManager->queueUnitForDeletion(otherUnit);
        }
    }

    bool Unit::isVisibleToTribe(Tribe *tribe) {
        return this->game->fogManager->tileIsClear(tribe, this->nextPosition->obj()->x, this->nextPosition->obj()->y);
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

    int Unit::getAdjustedDir(float x, float y)
    {
        auto dir = (std::atan2(y,x)*180/M_PI);
        if (dir <= 0)
            dir = -dir;
        else
            dir = 360-dir;
        if (MathUtils::Abs(0-dir)<1) return 0;
        if (MathUtils::Abs(90-dir)<1) return 2;
        if (MathUtils::Abs(180-dir)<1) return 4;
        if (MathUtils::Abs(270-dir)<1) return 6;
        if (MathUtils::Abs(360-dir)<1) return 0;

        int newDir =  ((MathUtils::Round((dir-45)/90))*2)+1;
        return newDir;
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

    void Unit::sendUpdate(PlayerData *playerData, bool overrideDirty)
    {
        if (!overrideDirty && !this->anyVarIsDirty(playerData)) return;
        std::stringstream msg;
        msg << "unit/1.0/update|" << id << "|";
        bool firstVar = true;

        if (overrideDirty || this->nextPosition->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->nextPosition->serialize();
        }

        if (overrideDirty || this->moveTarget->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->moveTarget->serialize();
        }

        if (overrideDirty || this->animationState->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->animationState->serialize();
        }

        if (overrideDirty || this->health->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->health->serialize();
        }

        if (overrideDirty || this->bleed->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->bleed->serialize();
        }

        if (overrideDirty || this->syncedTargetUnitId->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            msg << this->syncedTargetUnitId->serialize();
        }

        if (overrideDirty || this->gatherYield->isDirty())
        {
            if (firstVar) { firstVar = false; } else { msg << "&"; }
            if (playerData == this->tribe->playerData) {
                msg << this->gatherYield->serialize();
            } else {
                msg << "gatherYield:";
                msg << std::to_string(this->gatherYield->obj()->x);
            }
        }

        if (this->constructionQueue != nullptr)
        {
            if (playerData == this->tribe->playerData) {
                if (overrideDirty || this->constructionQueue->isDirty()) {
                    if (firstVar) { firstVar = false; } else { msg << "&"; }
                    msg << this->constructionQueue->getUpdate(overrideDirty);
                }
            }
        }

        if (this->inventory != nullptr)
        {
            if (playerData == this->tribe->playerData) {
                if (overrideDirty || this->inventory->isDirty()) {
                    if (firstVar) { firstVar = false; } else { msg << "&"; }
                    msg << this->inventory->getUpdate(overrideDirty);
                }
            }
        }

        //next synced variable should follow this format
//        if (this->nextPosition->isDirty())
//        {
//            msg << "&" << this->nextPosition->serialize();
//        }

        msg << "\r\n";
        GameServer::get().queueMessage(playerData, msg.str());
    }

    bool Unit::anyVarIsDirty(PlayerData *playerData)
    {
        bool sendingToControllerPlayer = playerData == this->tribe->playerData;

        if (this->nextPosition->isDirty()) return true;
        if (this->moveTarget->isDirty()) return true;
        if (this->animationState->isDirty()) return true;
        if (this->health->isDirty()) return true;
        if (this->syncedTargetUnitId->isDirty()) return true;
        if (this->bleed->isDirty()) return true;
        if (this->gatherYield->isDirty()) return true;
        if (sendingToControllerPlayer && this->constructionQueue!=nullptr && this->constructionQueue->isDirty()) return true;
        if (sendingToControllerPlayer && this->inventory!=nullptr && this->inventory->isDirty()) return true;
        //more synced vars here
        return false;
    }

    void Unit::cleanAllVars() {
        if (anyVarIsDirty(this->tribe->playerData)) {
            this->nextPosition->clean();
            this->moveTarget->clean();
            this->animationState->clean();
            this->health->clean();
            this->bleed->clean();
            this->syncedTargetUnitId->clean();
            this->gatherYield->clean();
            if (this->constructionQueue != nullptr) this->constructionQueue->clean();
            if (this->inventory != nullptr) this->inventory->clean();
            //more synced vars here
        }
    }

    bool Unit::canMove() {
        return this->unitTemplate->walkMax > 0;
    }

    bool Unit::canAttack() {
        return this->unitTemplate->acquisition > 0;
    }

    void Unit::setAnimationStateHeading(Unit *targetUnit)
    {
        int newHeading;
        if (targetUnit->unitTemplate->tileWidth == 1 && targetUnit->unitTemplate->tileHeight == 1)
        {
            newHeading = getDir(targetUnit->position->x - this->position->x,
                                targetUnit->position->y - this->position->y);
        }
        else
        {
            auto targetIsoBoxBase = IsoBoxCache::get().getIsoBox(targetUnit->unitTemplate->tileWidth, targetUnit->unitTemplate->tileHeight);

            float avgX = 0; float avgY = 0;
            for(auto isoBoxCoord : targetIsoBoxBase->coordList)
            {
                avgX += targetUnit->position->x + isoBoxCoord->x;
                avgY += targetUnit->position->y + isoBoxCoord->y;
            }

            avgX /= targetIsoBoxBase->coordList.size();
            avgY /= targetIsoBoxBase->coordList.size();

            newHeading = getAdjustedDir(avgX - this->position->x,
                                avgY - this->position->y);
        }
        if (this->animationState->obj()->GetHeading() != newHeading)
            this->animationState->dirtyObj()->SetHeading(newHeading);
    }

    void Unit::trainUnit(UnitTemplate *unitTemplate) {
        if (!this->constructionQueue->atMaxQueue()){
            this->constructionQueue->enqueue(unitTemplate);
        }
    }

    std::list<Point*> shuffle(std::list<Point*>& lst ) // shuffle contents of a list
    {
        // create a vector of (wrapped) references to elements in the list
        // http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
        std::vector< std::reference_wrapper<Point* > > vec( lst.begin(), lst.end() ) ;

        // shuffle (the references in) the vector
        std::shuffle( vec.begin(), vec.end(), std::mt19937{ std::random_device{}() } ) ;

        // copy the shuffled sequence into a new list
        std::list<Point*> shuffled_list {  vec.begin(), vec.end() } ;

        // swap the old list with the shuffled list
        return shuffled_list;
    }

    void Unit::finishTraining(UnitTemplate *unitTemplate)
    {
        auto outsideIsoBox = IsoBoxCache::get().getHollowIsoBox(this->unitTemplate->tileWidth+2, this->unitTemplate->tileHeight+2);
        std::list<Point*> isoBoxCoords = shuffle(outsideIsoBox->coordList);

        int spawnX;
        int spawnY;
        bool foundSpawn = false;
        while (true)
        {
            if (isoBoxCoords.size() <= 0)
            {
                    break;
            }

            Point* spawnPoint = isoBoxCoords.front();
            isoBoxCoords.pop_front();
            spawnX = this->position->x + spawnPoint->x;
            spawnY = this->position->y + spawnPoint->y;
            if (this->game->unitManager->getUnitFromGrid(spawnX, spawnY) != nullptr) continue;
            auto tileAtSpawn = this->game->tileManager->getTileAt(spawnX, spawnY);
            if (tileAtSpawn == nullptr || !tileAtSpawn->walkable) continue;
            foundSpawn = true;
            break;
        }

        if (foundSpawn)
        {
            this->game->unitManager->createUnit(spawnX, spawnY, this->tribe, unitTemplate);
        }
    }
}