//
// Created by connor on 9/12/18.
//

#include "UnitTemplateCatalog.h"
#include "SpecificUnit/Manafount.h"

namespace DsprGameServer
{
    UnitTemplateCatalog::UnitTemplateCatalog()
    {
        initWorker();
        initTemple();
        initManafount();
        initAshwalker();
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
        this->worker->setWalkSpeed(2,3);
        this->worker->maxHealth = 420;
        this->worker->buildTime = 300;
        this->worker->canGather = true;
        this->worker->hasInventory = true;

        this->templateMap.emplace(this->worker->index, this->worker);
    }

    void UnitTemplateCatalog::initTemple() {
        this->temple = new UnitTemplate(1);
        this->temple->sight = 12;
        this->temple->acquisition = 0;
        this->temple->setWalkSpeed(0,0);
        this->temple->tileWidth = 5;
        this->temple->tileHeight = 5;
        this->temple->maxHealth = 1500;
        this->temple->hasConstructionQueue = true;
        this->temple->buildableUnits = new std::set<int>();
        this->temple->buildableUnits->insert(0);
        this->temple->buildTime = 1000;
        this->temple->hasRallyPoint = true;

        this->templateMap.emplace(this->temple->index, this->temple);
    }

    void UnitTemplateCatalog::initManafount() {
        this->manafount = new UnitTemplate(2);
        this->manafount->setWalkSpeed(0,0);
        this->manafount->tileWidth = 2;
        this->manafount->tileHeight = 2;
        this->manafount->isInvincible = true;
        this->manafount->isGatherable = true;
        this->manafount->createSpecificUnitFunction = [&](Unit* unit) {
            return new Manafount(unit);
        };

        this->templateMap.emplace(this->manafount->index, this->manafount);
    }

    void UnitTemplateCatalog::initAshwalker() {
        this->ashwalker = new UnitTemplate(3);
        this->ashwalker->sight = 8;
        this->ashwalker->acquisition = 8;
        this->ashwalker->setWalkSpeed(4,6);
        this->ashwalker->maxHealth = 340;

        this->templateMap.emplace(this->ashwalker->index, this->ashwalker);
    }
}