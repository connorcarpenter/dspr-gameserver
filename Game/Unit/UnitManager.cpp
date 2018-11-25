//
// Created by connor on 7/31/18.
//

#include <sstream>
#include <iostream>
#include "UnitManager.h"
#include "../../Math/MathUtils.h"
#include "../../GameServer.h"
#include "../../Pathfinding/AStarPathfinder.h"
#include "OrderGroup.h"
#include "../TribeManager.h"
#include "../../PlayerData.h"
#include "UnitTemplateCatalog.h"
#include "../IsoBox/IsoBoxCache.h"
#include "../Item/ItemManager.h"
#include "../FogManager.h"

namespace DsprGameServer
{
    UnitManager::UnitManager(Game *game)
    {
        this->game = game;

        this->unitGrid = new PtrIsoGrid<Unit*>();
        this->unitGrid->initialize(this->game->tileManager->width * 2, this->game->tileManager->height * 2);

        this->endPosGrid = new PrimIsoGrid<bool>();
        this->endPosGrid->initialize(this->game->tileManager->width * 2, this->game->tileManager->height * 2);
        this->endPosGrid->forEachCoord([&](int x, int y){
           this->endPosGrid->set(x,y,false);
        });
    }

    void UnitManager::initializeFirstUnits()
    {
        //red tribe
        int i = 0;
        //for (int i = 0; i<3; i++)
            for (int j = 0; j<3;j++)
                createUnit((2 + j) * 2, (8 + i) * 2, this->game->tribeManager->tribeA, this->game->unitTemplateCatalog->worker);

        createUnit((3) * 2, (3) * 2, this->game->tribeManager->tribeA, this->game->unitTemplateCatalog->temple);
        createUnit((7) * 2, (7) * 2, this->game->tribeManager->neutralTribe, this->game->unitTemplateCatalog->manafount);

        //ashwalkers
        for (int i = 0; i<2; i++)
        for (int j = 0; j<2;j++) {
            createUnit((15+i) * 2, (15+j) * 2, this->game->tribeManager->tribeCreep, this->game->unitTemplateCatalog->ashwalker);
        }

        //blue tribe
        //for (int i = 0; i<3; i++)
            for (int j = 0; j<3;j++)
                createUnit((40 + i) * 2, (40 + j) * 2, this->game->tribeManager->tribeB, this->game->unitTemplateCatalog->worker);

        createUnit((52) * 2, (45) * 2, this->game->tribeManager->tribeB, this->game->unitTemplateCatalog->temple);
    }

    UnitManager::~UnitManager()
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            delete unit;
        }

        delete unitGrid;
        delete endPosGrid;

        for(const auto& mapPair : this->playerToUnitsAwareOfMap)
        {
            std::set<Unit*>* set = mapPair.second;
            delete set;
        }
    }

    void UnitManager::initSendAllUnits(PlayerData *playerData)
    {
        for(const auto& unitPair : unitMap)
        {
            int id = unitPair.first;
            Unit* unit = unitPair.second;

            if (unit->isVisibleToTribe(playerData->getTribe()))
            {
                this->makePlayerAwareOfUnit(playerData, unit);
            }
        }
    }

    Unit * UnitManager::createUnit(int x, int y, Tribe *tribe, UnitTemplate* unitTemplate)
    {
        Unit* newUnit = new Unit(this->game, getFreeUnitId(), tribe, x, y, unitTemplate);
        unitMap.insert(std::pair<int, Unit*>(newUnit->id, newUnit));
        return newUnit;
    }

    int UnitManager::getFreeUnitId() {
        if (!unusedIds.empty())
        {
            auto freeId = unusedIds.front();
            unusedIds.pop();
            return freeId;
        }

        return (int) unitMap.size()+1;
    }

    void UnitManager::receiveMoveOrder(const std::list<int> &idList, int tileX, int tileY)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::Move);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0){
                Unit* unit = unitMap.at(i);
                if (!unit->canMove()) continue;

                unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        if (unitPositionsList.empty()) return;

        auto path = this->game->pathfinder->findPath(unitPositionsList, tileX, tileY, false);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    if (!unit->canMove()) continue;
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveRallyPointOrder(const std::list<int> &idList, int tileX, int tileY)
    {
        for (const auto &i : idList) {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->unitTemplate->hasRallyPoint)continue;
                unit->rallyPoint->dirtyObj()->Set(tileX, tileY);
            }
        }
    }

    void UnitManager::receiveRallyUnitOrder(const std::list<int> &idList, int targetId)
    {
        for (const auto &i : idList) {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->unitTemplate->hasRallyPoint)continue;
                unit->rallyUnitId->dirtyObj()->Set(targetId);
            }
        }
    }

    void UnitManager::receiveAttackMoveOrder(const std::list<int> &idList, int tileX, int tileY)
    {
        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::AttackMove);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->canMove() || !unit->canAttack()) continue;
                unitPositionsList.emplace_back(
                        std::pair<int, int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        if (unitPositionsList.empty()) return;

        auto path = this->game->pathfinder->findPath(unitPositionsList, tileX, tileY, false);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    if (!unit->canMove() || !unit->canAttack()) continue;
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveFollowOrder(const std::list<int> &idList, int targetUnitId)
    {
        if (unitMap.count(targetUnitId) == 0) return;

        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::Follow);

        auto targetUnit = unitMap.at(targetUnitId);
        newOrderGroup->setTargetUnit(targetUnit);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->canMove()) continue;
                unitPositionsList.emplace_back(std::pair<int, int>(unit->position->x, unit->position->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        if (unitPositionsList.empty()) return;

        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                     targetUnit->position->y, false);
        if (path != nullptr) {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    if (!unit->canMove()) continue;
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveAttackTargetOrder(const std::list<int> &idList, int targetUnitId)
    {
        if (unitMap.count(targetUnitId) == 0) return;

        std::list<std::pair<int, int>> unitPositionsList;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::AttackTargetStrong);
        auto targetUnit = unitMap.at(targetUnitId);
        newOrderGroup->setTargetUnit(targetUnit);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->canAttack()) continue;
                unitPositionsList.emplace_back(std::pair<int, int>(unit->position->x, unit->position->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        if (unitPositionsList.empty()) return;
        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                     targetUnit->position->y, true);
        if (path != nullptr)
        {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    if (!unit->canAttack()) continue;
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receiveStopOrder(const std::list<int> &idList)
    {
        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::Move);

        for (const auto &i : idList)
            if (unitMap.count(i) != 0) {
                auto unit = unitMap.at(i);
                if (!unit->canMove() && !unit->canAttack()) continue;
                unit->stop(newOrderGroup);
            }
    }

    void UnitManager::receiveHoldOrder(const std::list<int> &idList)
    {
        for (const auto &i : idList)
            if (unitMap.count(i) != 0) {
                auto unit = unitMap.at(i);
                if (!unit->canMove() || !unit->canAttack()) continue;
                unit->hold();
            }
    }

    void UnitManager::receiveTrainOrder(const std::list<int> &idList, int unitTemplateIndex)
    {
        for (const auto &i : idList)
            if (unitMap.count(i) != 0) {
                auto unit = unitMap.at(i);
                if (!unit->unitTemplate->hasConstructionQueue) continue;
                if (unit->unitTemplate->buildableUnits == nullptr) continue;
                if (unit->unitTemplate->buildableUnits->count(unitTemplateIndex) == 0) continue;
                auto unitTemplate = this->game->unitTemplateCatalog->getTemplateFromIndex(unitTemplateIndex);
                unit->trainUnit(unitTemplate);
            }
    }

    void UnitManager::receiveCancelTrainOrder(std::list<int> idList, int queueIndex) {
        for (const auto &i : idList)
            if (unitMap.count(i) != 0) {
                auto unit = unitMap.at(i);
                if (!unit->unitTemplate->hasConstructionQueue) continue;
                unit->cancelTrainUnit(queueIndex);
            }
    }

    void UnitManager::receiveGatherOrder(const std::list<int> &idList, int targetUnitId)
    {
        if (unitMap.count(targetUnitId) == 0) return;

        std::list<std::pair<int, int>> unitPositionsList;

        auto targetUnit = unitMap.at(targetUnitId);
        if (!targetUnit->unitTemplate->isGatherable)return;

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::Gather);
        newOrderGroup->setTargetUnit(targetUnit);

        for (const auto& i : idList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->unitTemplate->canGather) continue;
                unitPositionsList.emplace_back(std::pair<int, int>(unit->position->x, unit->position->y));
                unit->setOrderGroup(newOrderGroup);
            }
        }

        if (unitPositionsList.empty()) return;
        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                     targetUnit->position->y, true);
        if (path != nullptr)
        {
            newOrderGroup->setPath(path);

            for (const auto &i : idList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    if (!unit->unitTemplate->canGather) continue;
                    unit->startPath();
                }
            }
        }
    }

    void UnitManager::receivePickupOrder(std::list<int> unitIdList, int targetItemId) {
        if (!this->game->itemManager->itemExists(targetItemId)) return;

        std::list<std::pair<int, int>> unitPositionsList;

        auto targetItem = this->game->itemManager->getItem(targetItemId);

        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::Pickup);
        newOrderGroup->setTargetItem(targetItem);

        for (const auto& i : unitIdList)
        {
            if (unitMap.count(i) != 0) {
                Unit *unit = unitMap.at(i);
                if (!unit->unitTemplate->hasInventory) continue;
                unitPositionsList.emplace_back(std::pair<int, int>(unit->position->x, unit->position->y));
                unit->setOrderGroup(newOrderGroup);
                break;
            }
        }

        if (unitPositionsList.empty()) return;
        auto path = this->game->pathfinder->findPath(unitPositionsList, targetItem->position->x,
                                                     targetItem->position->y, true);
        if (path != nullptr)
        {
            newOrderGroup->setPath(path);

            for (const auto &i : unitIdList) {
                if (unitMap.count(i) != 0) {
                    Unit *unit = unitMap.at(i);
                    if (!unit->unitTemplate->hasInventory) continue;
                    unit->startPath();
                    break;
                }
            }
        }
    }

    void UnitManager::receiveItemSwapOrder(int unitId, int beforeSlotIndex, int afterSlotIndex)
    {
        if (unitMap.count(unitId) == 0) return;

        Unit *unit = unitMap.at(unitId);
        if (!unit->unitTemplate->hasInventory) return;
        unit->inventory->swapSlots(beforeSlotIndex, afterSlotIndex);
    }

    void UnitManager::receiveItemDropOrder(int unitId, int slotIndex, int x, int y)
    {
        if (unitMap.count(unitId) == 0) return;

        Unit *unit = unitMap.at(unitId);
        if (!unit->unitTemplate->hasInventory) return;
        if (!unit->canMove())return;

        ///
        std::list<std::pair<int, int>> unitPositionsList;
        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::ItemDrop);
        unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
        unit->setOrderGroup(newOrderGroup);

        auto path = this->game->pathfinder->findPath(unitPositionsList, x, y, false);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);
            unit->startPath();
            unit->inventory->itemToDrop = unit->inventory->getItem(slotIndex);
        }
        ///
    }

    void UnitManager::receiveItemGiveOrder(int unitId, int slotIndex, int targetUnitId)
    {
        if (unitMap.count(unitId) == 0) return;

        Unit *unit = unitMap.at(unitId);
        if (!unit->unitTemplate->hasInventory) return;
        if (!unit->canMove())return;

        Unit *targetUnit = unitMap.at(targetUnitId);
        if (!targetUnit->unitTemplate->hasInventory) return;
        if (unit->tribe != targetUnit->tribe) return;

        ///
        std::list<std::pair<int, int>> unitPositionsList;
        auto newOrderGroup = std::make_shared<OrderGroup>(this->game, UnitOrderType::ItemGive);
        newOrderGroup->setTargetUnit(targetUnit);
        unitPositionsList.emplace_back(std::pair<int,int>(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y));
        unit->setOrderGroup(newOrderGroup);

        auto path = this->game->pathfinder->findPath(unitPositionsList, targetUnit->position->x,
                                                     targetUnit->position->y, false);
        if (path!= nullptr) {
            newOrderGroup->setPath(path);
            unit->startPath();
            unit->inventory->itemToDrop = unit->inventory->getItem(slotIndex);
        }
        ///
    }

    void UnitManager::updateUnits()
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            unit->update();
        }
    }

    void UnitManager::sendUnitUpdates(PlayerData *playerData)
    {
        for(const auto& unitPair : unitMap)
        {
            Unit* unit = unitPair.second;
            bool playerIsAwareOfUnit = this->playerIsAwareOfUnit(playerData, unit);

            if (playerIsAwareOfUnit)
            {
                if (!unit->isVisibleToTribe(playerData->getTribe()))
                {
                    this->makePlayerUnawareOfUnit(playerData, unit);
                    playerIsAwareOfUnit = false;
                }
            }
            else
            {
                if (unit->isVisibleToTribe(playerData->getTribe()))
                {
                    this->makePlayerAwareOfUnit(playerData, unit);
                    playerIsAwareOfUnit = true;
                }
            }

            if (playerIsAwareOfUnit)
            {
                unit->sendUpdate(playerData, false);
            }
        }
    }

    void UnitManager::cleanAllUnits()
    {
        for(const auto& unitPair : unitMap)
        {
            unitPair.second->cleanAllVars();
        }
    }

    void UnitManager::setUnitInGrid(int x, int y, Unit* unit, UnitTemplate* unitTemplate) {
        //return this->unitGrid->set(x,y, unit);

        if (unitTemplate->tileWidth == 1 && unitTemplate->tileHeight==1)
        {
            this->unitGrid->set(x,y, unit);
        }
        else
        {
            auto unitIsoBoxBase = IsoBoxCache::get().getIsoBox(unitTemplate->tileWidth, unitTemplate->tileHeight);

            for(auto isoBoxCoord : unitIsoBoxBase->coordList)
            {
                this->unitGrid->set(x + isoBoxCoord->x, y + isoBoxCoord->y, unit);
            }
        }
    }

    Unit* UnitManager::getUnitFromGrid(int x, int y){
        return this->unitGrid->get(x,y);
    }

    void UnitManager::removeUnitFromGrid(Unit *unit) {
        setUnitInGrid(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y, nullptr, unit->unitTemplate);
    }

    void UnitManager::addUnitToGrid(Unit *unit) {
        setUnitInGrid(unit->nextPosition->obj()->x, unit->nextPosition->obj()->y, unit, unit->unitTemplate);
    }

    bool UnitManager::getEndPosAtCoord(int x, int y) {
        return this->endPosGrid->get(x, y);
    }

    void UnitManager::addEndPosAtCoord(int x, int y) {
        this->endPosGrid->set(x, y, true);
    }

    void UnitManager::removeEndPosAtCoord(int x, int y) {
        this->endPosGrid->set(x, y, false);
    }

    void UnitManager::deleteUnits() {
        for(auto unit : this->unitsToDelete)
        {
            std::cout << "deleting unit, w/ id: " << unit->id << "\r\n";

            //clean up
            this->removeUnitFromGrid(unit);
            this->unitMap.erase(unit->id);
            this->unusedIds.push(unit->id);

            //setup sending the deletions
            int showDeath = (unit->health->obj()->Get() <= 0) ? 1 : 0;
            this->unitDeletionsToSend.emplace(std::make_pair(unit->id, showDeath));

            //remove this unit from all players aware of it
            for(const auto& playerUnitPair : this->playerToUnitsAwareOfMap)
            {
                if (playerUnitPair.second != nullptr)
                    playerUnitPair.second->erase(unit);
            }

            //actually delete
            delete unit;

            std::cout << "deleted unit: " << unit << "\r\n";
        }

        this->unitsToDelete.clear();
    }

    void UnitManager::sendUnitDeletes(PlayerData *playerData)
    {
        for(auto unitId : this->unitDeletionsToSend)
        {
            DsprMessage::UnitDeleteMsgV1 unitDeleteMsgV1;
            unitDeleteMsgV1.id.set(unitId.first);
            unitDeleteMsgV1.dead.set(unitId.second);
            auto clientMsg = unitDeleteMsgV1.getToClientMessage();
            auto packedMsg = clientMsg->Pack();
            GameServer::get().queueMessage(playerData, packedMsg);
        }
    }

    void UnitManager::queueUnitForDeletion(Unit* deletedUnit){
        this->unitsToDelete.emplace(deletedUnit);
    }

    Unit *UnitManager::getUnitWithId(int unitId) {
        if (this->unitMap.count(unitId) <= 0)return nullptr;
        return this->unitMap.at(unitId);
    }

    void UnitManager::finishSendUnitDeletes() {
        this->unitDeletionsToSend.clear();
    }

    void UnitManager::makePlayerAwareOfUnit(PlayerData *playerData, Unit *unit)
    {
        DsprMessage::UnitCreateMsgV1 unitCreateMsgV1;
        unitCreateMsgV1.id.set(unit->id);
        unitCreateMsgV1.x.set(unit->position->x);
        unitCreateMsgV1.y.set(unit->position->y);
        unitCreateMsgV1.tribeIndex.set(unit->tribe->index);
        unitCreateMsgV1.templateIndex.set(unit->unitTemplate->index);
        auto clientMsg = unitCreateMsgV1.getToClientMessage();
        auto packedMsg = clientMsg->Pack();
        GameServer::get().queueMessage(playerData, packedMsg);

        unit->sendUpdate(playerData, true);

        std::set<Unit*>* unitSet = this->playerToUnitsAwareOfMap.at(playerData);
        unitSet->emplace(unit);
    }

    void UnitManager::makePlayerUnawareOfUnit(PlayerData *playerData, Unit *unit)
    {
        DsprMessage::UnitDeleteMsgV1 unitDeleteMsgV1;
        unitDeleteMsgV1.id.set(unit->id);
        unitDeleteMsgV1.dead.set(0);
        auto clientMsg = unitDeleteMsgV1.getToClientMessage();
        auto packedMsg = clientMsg->Pack();
        GameServer::get().queueMessage(playerData, packedMsg);

        std::set<Unit*>* unitSet = this->playerToUnitsAwareOfMap.at(playerData);
        unitSet->erase(unit);
    }

    void UnitManager::addPlayer(PlayerData *playerData) {
        this->playerToUnitsAwareOfMap.emplace(std::make_pair(playerData, new std::set<Unit*>()));
    }

    void UnitManager::removePlayer(PlayerData *playerData) {
        if (this->playerToUnitsAwareOfMap.count(playerData) <= 0) return;
        auto unitSet = this->playerToUnitsAwareOfMap.at(playerData);
        delete unitSet;
        this->playerToUnitsAwareOfMap.erase(playerData);
    }

    bool UnitManager::playerIsAwareOfUnit(PlayerData *playerData, Unit *unit) {
        auto playerUnitAwareSet = this->playerToUnitsAwareOfMap.at(playerData);
        return playerUnitAwareSet->count(unit) != 0;
    }

    void UnitManager::createProjectile(int fromX, int fromY, int toX, int toY, int index, Unit *unit) {
        auto newPrjctl = new Projectile(this->game, fromX, fromY, toX, toY, index, unit);
        this->projectileCreationSet.insert(newPrjctl);
    }

    void UnitManager::updateProjectiles() {
        std::set<Projectile*>* prjctlsToDelete = nullptr;
        for(auto prjctl : this->projectileSet)
        {
            prjctl->update();
            if (prjctl->done)
            {
                if (prjctlsToDelete == nullptr)
                {
                    prjctlsToDelete = new std::set<Projectile*>();
                }

                prjctlsToDelete->insert(prjctl);
            }
        }

        if (prjctlsToDelete != nullptr)
        {
            for (auto prjctl : *prjctlsToDelete)
            {
                this->projectileSet.erase(prjctl);
                delete prjctl;
            }
            delete prjctlsToDelete;
        }
    }

    void UnitManager::damageUnitAtPoint(int fromX, int fromY, int x, int y, int damage) {
        auto unitToDamage = this->getUnitFromGrid(x,y);
        if (unitToDamage == nullptr) return;
        unitToDamage->receiveDamage(fromX, fromY, damage);
    }

    void UnitManager::sendProjectiles(PlayerData *const &playerData) {
        for (auto prjctl : this->projectileCreationSet)
        {
            if (playerIsAwareOfUnit(playerData, prjctl->unit)) continue;

            if (this->game->fogManager->tileIsClear(playerData->getTribe(), prjctl->fromCoord->x, prjctl->fromCoord->y) ||
                    this->game->fogManager->tileIsClear(playerData->getTribe(), prjctl->toCoord->x, prjctl->toCoord->y))
            {
                DsprMessage::ProjectileCreateMsgV1 projectileCreateMsgV1;
                projectileCreateMsgV1.from.add(prjctl->fromCoord->x);
                projectileCreateMsgV1.from.add(prjctl->fromCoord->y);
                projectileCreateMsgV1.to.add(prjctl->toCoord->x);
                projectileCreateMsgV1.to.add(prjctl->toCoord->y);
                projectileCreateMsgV1.index.set(prjctl->index);
                auto clientMsg = projectileCreateMsgV1.getToClientMessage();
                auto packedMsg = clientMsg->Pack();
                GameServer::get().queueMessage(playerData, packedMsg);
            }
        }
    }

    void UnitManager::cleanProjectiles() {
        for (auto prjctl : this->projectileCreationSet)
        {
            this->projectileSet.insert(prjctl);
        }

        this->projectileCreationSet.clear();
    }
}