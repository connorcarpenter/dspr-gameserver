#pragma once

//
// Created by connor on 8/6/18.
//

namespace DsprGameServer
{
    class PathNode
    {
    public:

        PathNode(int x, int y, PathNode* parent, float parDis, int tarX, int tarY);

        PathNode(PathNode *other);

        int getId() const;
        static int getId(int x, int y);
        float getF()const;
        float getH(int endX, int endY);

        int x;
        int y;
        float g = 0;
        float h = 0;
        PathNode* parent = nullptr;
    };

    class PathNodeComparator
    {
    public:
        int operator() (const PathNode* p1, const PathNode* p2)
        {
            return p1->getF() > p2->getF();
        }
    };
}