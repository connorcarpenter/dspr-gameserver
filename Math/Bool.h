#pragma once

//
// Created by connor on 8/29/18.
//

#include <string>

namespace DsprGameServer{

    class Bool {
    public:
        Bool(bool newValue);
        void Set(bool newBool);
        bool Get();
        std::string serialize();

        bool value;

        bool Flip();
    };
}