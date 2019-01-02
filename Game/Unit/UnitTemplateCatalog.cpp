//
// Created by connor on 9/12/18.
//

#include "UnitTemplateCatalog.h"
#include "SpecificUnit/Manafount.h"
#include "SpecificUnit/TempleBuilding.h"
#include "SpecificUnit/TempleFlying.h"

namespace DsprGameServer
{
    UnitTemplateCatalog::UnitTemplateCatalog()
    {
        initWorker();
        initTempleBuilding();
        initTempleFlying();
        initManafount();
        initAshwalker();
        initRift();
    }

    UnitTemplateCatalog::~UnitTemplateCatalog()
    {
        for(const auto& unitTemplatePair : this->templateMap)
        {
            UnitTemplate* unitTemplate = unitTemplatePair.second;
            delete unitTemplate;
        }
    }

    UnitTemplate* UnitTemplateCatalog::getTemplateFromIndex(int index) {
        if (this->templateMap.count(index) <= 0) return nullptr;
        return this->templateMap.at(index);
    }

    void UnitTemplateCatalog::initWorker() {
        this->worker = new UnitTemplate(0);
        this->worker->sight = 6;
        this->worker->acquisition = 6;
        this->worker->range = 2;//make sure this is multiples of 2... to accomodate for diagonal tiles
        this->worker->minDamage = 10;
        this->worker->maxDamage = 15;
        this->worker->setWalkSpeed(2,3);
        this->worker->maxHealth = 420;
        this->worker->buildTime = 50;
        this->worker->manaCost = 50;
        this->worker->canGather = true;
        this->worker->hasInventory = true;

        this->templateMap.emplace(this->worker->index, this->worker);
    }



    void UnitTemplateCatalog::initManafount() {
        this->manafount = new UnitTemplate(2);
        this->manafount->setWalkSpeed(0,0);
        this->manafount->tileWidth = 2;
        this->manafount->tileHeight = 2;
        this->manafount->isInvincible = true;
        this->manafount->isGatherable = true;
        this->manafount->acquisition = 0;
        this->manafount->createSpecificUnitFunction = [&](Unit* unit) {
            return new Manafount(unit);
        };

        this->templateMap.emplace(this->manafount->index, this->manafount);
    }

    void UnitTemplateCatalog::initAshwalker() {
        this->ashwalker = new UnitTemplate(3);
        this->ashwalker->sight = 8;
        this->ashwalker->acquisition = 8;
        this->ashwalker->range = 2;
        this->ashwalker->minDamage = 10;
        this->ashwalker->maxDamage = 15;
        this->ashwalker->setWalkSpeed(4,6);
        this->ashwalker->maxHealth = 340;

        this->templateMap.emplace(this->ashwalker->index, this->ashwalker);
    }

    void UnitTemplateCatalog::initTempleBuilding() {
        this->templeBuilding = new UnitTemplate(1);
        this->templeBuilding->sight = 12;
        this->templeBuilding->acquisition = 0;
        this->templeBuilding->setWalkSpeed(0,0);
        this->templeBuilding->tileWidth = 5;
        this->templeBuilding->tileHeight = 5;
        this->templeBuilding->maxHealth = 1500;
        this->templeBuilding->hasConstructionQueue = true;
        this->templeBuilding->buildableUnits = new std::set<int>();
        this->templeBuilding->buildableUnits->insert(0);
        this->templeBuilding->buildTime = 1000;
        this->templeBuilding->hasRallyPoint = true;
        this->templeBuilding->createSpecificUnitFunction = [&](Unit* unit) {
            return new TempleBuilding(unit);
        };

        this->templateMap.emplace(this->templeBuilding->index, this->templeBuilding);
    }

    void UnitTemplateCatalog::initTempleFlying() {
        this->templeFlying = new UnitTemplate(4);
        this->templeFlying->sight = 12;
        this->templeFlying->acquisition = 0;
        this->templeFlying->setWalkSpeed(20,30);
        this->templeFlying->tileWidth = 1;
        this->templeFlying->tileHeight = 1;
        this->templeFlying->maxHealth = 1500;
        this->templeFlying->buildTime = 1000;
        this->templeFlying->createSpecificUnitFunction = [&](Unit* unit) {
            return new TempleFlying(unit);
        };

        this->templateMap.emplace(this->templeFlying->index, this->templeFlying);
    }

    void UnitTemplateCatalog::initRift() {
        this->rift = new UnitTemplate(5);
        this->rift->sight = 0;
        this->rift->acquisition = 0;
        this->rift->setWalkSpeed(0,0);
        this->rift->tileWidth = 5;
        this->rift->tileHeight = 5;
        this->rift->isInvincible = true;

        this->templateMap.emplace(this->rift->index, this->rift);
    }
}