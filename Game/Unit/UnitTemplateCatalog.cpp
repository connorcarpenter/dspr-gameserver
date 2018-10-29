//
// Created by connor on 9/12/18.
//

#include "UnitTemplateCatalog.h"

namespace DsprGameServer
{
    UnitTemplateCatalog::UnitTemplateCatalog()
    {
        this->worker = new UnitTemplate(0);
        this->worker->sight = 6;
        this->worker->acquisition = 6;
        this->worker->setWalkSpeed(2,3);
        this->worker->maxHealth = 420;
        this->worker->buildTime = 100;
        this->templateMap.emplace(this->worker->index, this->worker);

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
        this->templateMap.emplace(this->temple->index, this->temple);

        this->manafount = new UnitTemplate(2);
        this->manafount->setWalkSpeed(0,0);
        this->manafount->tileWidth = 2;
        this->manafount->tileHeight = 2;
        this->manafount->isInvincible = true;
        this->templateMap.emplace(this->manafount->index, this->manafount);
    }

    UnitTemplateCatalog::~UnitTemplateCatalog()
    {
        delete this->worker;
        delete this->temple;
    }

    UnitTemplate* UnitTemplateCatalog::getTemplateFromIndex(int index) {
        if (this->templateMap.count(index) <= 0) return nullptr;
        return this->templateMap.at(index);
    }


}