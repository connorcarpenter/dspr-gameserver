//
// Created by connor on 8/2/18.
//

#include "FloatPoint.h"
#include "MathUtils.h"


namespace DsprGameServer
{
    FloatPoint::FloatPoint(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    bool FloatPoint::Equals(const FloatPoint* otherFloatPoint) {
        return (this->x == otherFloatPoint->x && this->y == otherFloatPoint->y);
    }

    void FloatPoint::Set(const FloatPoint* otherFloatPoint) {
        this->x = otherFloatPoint->x;
        this->y = otherFloatPoint->y;
    }

    void FloatPoint::Set(float x, float y) {
        this->x = x;
        this->y = y;
    }

    void FloatPoint::Add(float x, float y) {
        this->x += x;
        this->y += y;
    }

    float FloatPoint::Length() {
        if (x == 0 && y == 0) return 0;
        return MathUtils::Sqrt((this->x*this->x) + (this->y*this->y));
    }

    void FloatPoint::Normalize(float length) {
        auto mag = Length();
        if (mag > 0) {
            this->x *= length / mag;
            this->y *= length / mag;
        }
        else
        {
            this->x = length;
            this->y = 0;
        }
    }

    Point* FloatPoint::ToMovePoint()
    {
        auto dir = Direction();
        if (dir > 22.5 && dir <= 67.5) { return new Point(1, -1); }
        if (dir > 67.5 && dir <= 112.5) { return new Point(0, -1); }
        if (dir > 112.5 && dir <= 157.5) { return new Point(-1, -1); }
        if (dir > 157.5 && dir <= 202.5) { return new Point(-1, 0); }
        if (dir > 202.5 && dir <= 247.5) { return new Point(-1, 1); }
        if (dir > 247.5 && dir <= 292.5) { return new Point(0, 1); }
        if (dir > 292.5 && dir <= 337.5) { return new Point(1, 1); }
        return new Point(1, 0);
    }

    float FloatPoint::Direction()
    {
        auto dir = (std::atan2(y,x)*180/M_PI);
        if (dir <= 0) return -dir;
        return 360-dir;
    }


}
