#pragma once

//
// Created by connor on 8/2/18.
//

#include <string>
#include "Point.h"

namespace DsprGameServer
{
    class FloatPoint
    {
    public:
        FloatPoint(float x, float y);
        float x;
        float y;

        bool Equals(const FloatPoint *otherPoint);

        void Set(const FloatPoint *otherPoint);

        void Set(float x, float y);

        void Add(float x, float y);

        void Normalize(float length);

        float Length();

        Point *ToMovePoint();

        float Direction();
    };
}