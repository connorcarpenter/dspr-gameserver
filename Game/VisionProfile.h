#pragma once

//
// Created by connor on 12/9/18.
//

#include "Tribe.h"

namespace DsprGameServer {
    class VisionProfile {

    public:
        void addTribe(Tribe* tribe);
        std::set<Tribe*>& getTribes();
    private:
        std::set<Tribe*> tribes;
    };
}