//
// Created by connor on 8/29/18.
//

#include "Bool.h"

namespace DsprGameServer
{
    Bool::Bool(bool newValue) {
        this->value = newValue;
    }

    void Bool::Set(bool newBool) {
        this->value = newBool;
    }

    bool Bool::Get() {
        return this->value;
    }

    std::string Bool::serialize() {
        return std::to_string(this->value);
    }

    bool Bool::Flip() {
        this->value = !this->value;
    }
}

