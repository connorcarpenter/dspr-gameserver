//
// Created by connor on 9/12/18.
//

#include "UnitTemplateCatalog.h"

namespace DsprGameServer {
    UnitTemplateCatalog::UnitTemplateCatalog() {
        this->worker = new UnitTemplate(0);

        this->temple = new UnitTemplate(1);
    }

    UnitTemplateCatalog::~UnitTemplateCatalog() {
        delete this->worker;
        delete this->temple;
    }
}