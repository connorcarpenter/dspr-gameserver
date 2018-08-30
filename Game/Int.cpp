//
// Created by connor on 8/29/18.
//

#include "Int.h"

namespace DsprGameServer
{
    void Int::Set(int newInt) {
        this->currentInt = newInt;
    }

    int Int::Get() {
        return this->currentInt;
    }

    std::string Int::serialize() {
        return std::to_string(this->currentInt);
    }
}

