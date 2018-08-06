//
// Created by connor on 8/6/18.
//

#include "PathNode.h"
#include "../MathUtils.h"
#include "AStarPathfinder.h"

namespace DsprGameServer
{
    PathNode::PathNode(int x, int y, PathNode* parent, float parDis, int tarX, int tarY)
    {
        this->x = x;
        this->y = y;
        this->h = getH(tarX, tarY);

        if (parent == nullptr)
        {
            this->g = 0;
        }
        else
        {
            this->g = parent->g + parDis;
            this->parent = parent;
        }
    }

    PathNode::PathNode(PathNode *other)
    {
        this->x = other->x;
        this->y = other->y;
        this->g = other->g;
        this->h = other->h;
        this->parent = other->parent;
    }

    int PathNode::getId() const
    {
        return getId(this->x, this->y);
    }

    int PathNode::getId(int x, int y) {
        return (y*AStarPathfinder::getMapWidth()) + x;
    }

    float PathNode::getF()const {
        return this->g + this->h;
    }

    float PathNode::getH(int endX, int endY) {

        int d_max = MathUtils::Max(MathUtils::Abs(this->x - endX), MathUtils::Abs(this->y - endY));
        int d_min = MathUtils::Min(MathUtils::Abs(this->x - endX), MathUtils::Abs(this->y - endY));
        return (1.414f * d_min) + (d_max - d_min);
    }
}