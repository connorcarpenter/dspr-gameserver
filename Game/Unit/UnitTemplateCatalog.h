#pragma once

#include "UnitTemplate.h"

//
// Created by connor on 9/12/18.
//

namespace DsprGameServer {
    class UnitTemplateCatalog {
    public:
        UnitTemplateCatalog();
        ~UnitTemplateCatalog();
        UnitTemplate* worker = nullptr;
        UnitTemplate* temple = nullptr;
    };
}