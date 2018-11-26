#pragma once

#include <map>
#include "UnitTemplate.h"

//
// Created by connor on 9/12/18.
//

namespace DsprGameServer {
    class UnitTemplateCatalog {
    public:
        UnitTemplateCatalog();
        ~UnitTemplateCatalog();
        UnitTemplate* getTemplateFromIndex(int index);
        UnitTemplate* worker = nullptr;
        UnitTemplate* templeBuilding = nullptr;
        UnitTemplate* templeFlying = nullptr;
        UnitTemplate* manafount = nullptr;
        UnitTemplate *ashwalker = nullptr;
    private:
        std::map<int, UnitTemplate*> templateMap;

        void initWorker();
        void initTempleBuilding();
        void initTempleFlying();
        void initManafount();
        void initAshwalker();
    };
}