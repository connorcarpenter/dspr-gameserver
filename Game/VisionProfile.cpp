//
// Created by connor on 12/9/18.
//

#include "VisionProfile.h"

namespace DsprGameServer {

    std::set<Tribe*>& VisionProfile::getTribes() {
        return this->tribes;
    }

    void VisionProfile::addTribe(Tribe *tribe) {
        this->tribes.insert(tribe);
    }
}