//
// Created by connor on 9/12/18.
//

#include "UnitTemplateCatalog.h"

namespace DsprGameServer {
    UnitTemplateCatalog::UnitTemplateCatalog() {
        this->worker = new UnitTemplate();

        this->temple = new UnitTemplate();
    }

    UnitTemplateCatalog::~UnitTemplateCatalog() {
        delete this->worker;
        delete this->temple;
    }
}