//
// Created by connor on 8/29/18.
//

#include "Int.h"

namespace DsprGameServer
{
    Int::Int(int newValue) {
        this->value = newValue;
    }

    void Int::Set(int newInt) {
        this->value = newInt;
    }

    int Int::Get() {
        return this->value;
    }

    std::string Int::serialize() {
        return std::to_string(this->value);
    }

    void Int::Subtract(int subInt) {
        this->value -= subInt;
    }
}

