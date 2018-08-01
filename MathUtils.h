#pragma once

//
// Created by connor on 7/23/18.
//
#include <cstdlib>

namespace DsprGameServer {
    class MathUtils { ;
    public:
        static int getRandom(int max) {
            return std::rand() / ((RAND_MAX + 1u) / max);
        }
    };
}