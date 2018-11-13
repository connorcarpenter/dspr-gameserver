#pragma once

//
// Created by connor on 11/13/18.
//

#include "BodyPart.h"

namespace DsprGameServer {

    class ItemTemplate {
    public:
        ItemTemplate(int index);
        BodyPart wornOn = BodyPart::None;

        int index = -1;
    };
}