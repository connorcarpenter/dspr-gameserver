//
// Created by connor on 8/2/18.
//

#include "Point.h"
#include "MathUtils.h"


namespace DsprGameServer
{
    Point::Point(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    bool Point::Equals(const Point* otherPoint) {
        return (this->x == otherPoint->x && this->y == otherPoint->y);
    }

    void Point::Set(const Point* otherPoint) {
        this->x = otherPoint->x;
        this->y = otherPoint->y;
    }

    void Point::Set(int x, int y) {
        this->x = x;
        this->y = y;
    }

    std::string Point::serialize()
    {
        std::string str;
        str.append(std::to_string(this->x));
        str.append(",");
        str.append(std::to_string(this->y));
        return str;
    }

    void Point::Add(int x, int y) {
        this->x += x;
        this->y += y;
    }
}
