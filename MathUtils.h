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
        static int Sign(int value){
            if (value < 0)return -1;
            return 1;
        }
        static int SignOrZero(int value) {
            if (value == 0) return 0;
            return MathUtils::Sign(value);
        }
        static int Abs(int value) {
            return value * Sign(value);
        }
        static int Min(int value1, int value2) {
            return (value1 < value2) ? value1 : value2;
        }
        static int Max(int value1, int value2) {
            return (value1 > value2) ? value1 : value2;
        }
    };
}