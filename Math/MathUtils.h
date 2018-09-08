#pragma once

//
// Created by connor on 7/23/18.
//
#include <cstdlib>
#include <cmath>

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
        static float Sqrt(int value) {
            return (float) std::sqrt(value);
        }
        static float SqrtFloat(float value) {
            return std::sqrt(value);
        }

        static float Distance(int x1, int y1, int x2, int y2) {
            return SqrtFloat(PowerFloat(x2-x1,2)+PowerFloat(y2-y1, 2));
        }

        static int Ceiling(float value){
            if (value == (int) value) return (int) value;
            return (int) value + 1;
        }

        static int Pow(int value, int exponent){
            return (int) std::pow(value, exponent);
        }
        static float PowerFloat(float value, float power)
        {
            return std::pow(value, power);
        }
    };
}