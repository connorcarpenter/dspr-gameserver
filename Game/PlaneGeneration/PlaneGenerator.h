//
// Created by connor on 11/27/18.
//

#include "../../PtrIsoGrid.h"
#include "../Tile.h"
#include "../../PrimIsoGrid.h"
#include "../../Math/Point.h"
#include "../../Pathfinding/PathNode.h"
#include "../Game.h"
#include <list>
#include <queue>
#include <unordered_map>
#include <bits/unordered_map.h>
#include <bits/unordered_set.h>

namespace DsprGameServer {
    class PlaneGenerator {

    public:

        PlaneGenerator(Game* game);
        ~PlaneGenerator();
        void init();
        void fill(DsprGameServer::PtrIsoGrid<DsprGameServer::Tile*>* outputGrid);

        int getNeighborCount(DsprGameServer::PrimIsoGrid<bool> *grid, int x, int y);

        PrimIsoGrid<bool>* smooth(PrimIsoGrid<bool> *grid);

        PrimIsoGrid<bool> *planeGrid = nullptr;

        void finishPlane();

        Point playerStart = DsprGameServer::Point(0, 0);
        Point riftLocation = DsprGameServer::Point(0, 0);
        std::unordered_set<Point> manafountSet;
    private:

        Point findPlayerStart();

        Point getFurthestPointFromPoint(Point& point);
        Point getFurthestPointFromPointSet(std::unordered_set<Point> &pointSet, int limit = -1);

        std::__cxx11::list<DsprGameServer::PathNode *> *
    getNeighbors(DsprGameServer::PathNode *parent);

        void tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj,
                        float cost);

        void cleanUp(std::list<PathNode *> *nodes, std::unordered_map<int, PathNode *> *map,
                 std::priority_queue<PathNode *, std::vector<PathNode *>, PathNodeComparator> *heap,
                 std::unordered_map<int, PathNode *> *map2);

        const float straightCost = 2;
        const float diagonalCost = 3;
        Game *game;

        Point insulatePoint(Point point, int insulation);

        Point getPointSomeDisFromPoint(Point &point, int distance);
    };
}