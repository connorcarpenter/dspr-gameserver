//
// Created by connor on 11/27/18.
//

#include "PlaneGenerator.h"
#include "../../Math/MathUtils.h"
#include "../Circle/CircleCache.h"

namespace DsprGameServer {

    PlaneGenerator::PlaneGenerator(Game* game) {
        init();
        this->game = game;
    }

    PlaneGenerator::~PlaneGenerator() {
        if (this->planeGrid != nullptr)
            delete this->planeGrid;
    }

    void PlaneGenerator::init() {

    }

    void PlaneGenerator::fill(DsprGameServer::PtrIsoGrid<DsprGameServer::Tile *> *outputGrid)
    {
        //seed
        MathUtils::seedRandom(std::time(0));

        //initial randomization
        const int scaleDownRatio = 5;
        auto finalGrid = new PrimIsoGrid<bool>();
        finalGrid->initialize(((outputGrid->width/scaleDownRatio)/2)*2, ((outputGrid->height/scaleDownRatio)/2)*2);

        finalGrid->forEachCoord([finalGrid](int x, int y){
           bool val = (MathUtils::getRandom(100) <= 60);
            finalGrid->set(x,y,val);
        });

        //smoothing
        for(int i=0;i<7;i++) {
            finalGrid = this->smooth(finalGrid);
        }

        //scaling
        auto nextGrid = new PrimIsoGrid<bool>();
        nextGrid->initialize(outputGrid->width, outputGrid->height);
        nextGrid->forEachCoord([this, nextGrid, finalGrid](int x, int y) {
            int nx = x/scaleDownRatio; int ny=y/scaleDownRatio;
            if (!finalGrid->insideGrid(nx, ny))
            {
                if((MathUtils::getRandom(100) <= 50)) {
                    nx += 1;
                } else {
                    ny += 1;
                }
            }
            bool val = finalGrid->get(nx, ny);
            nextGrid->set(x, y, val);
        });

        delete finalGrid;
        finalGrid = nextGrid;

        //smoothing
        for(int i=0;i<5;i++) {
            finalGrid = this->smooth(finalGrid);
        }

        //convert to tiles
        outputGrid->forEachCoord([finalGrid, outputGrid](int x, int y){
           if (finalGrid->get(x,y))
           {
               int randomNumber = MathUtils::getRandom(100);
               int frameIndex = 5;
               if (randomNumber < 99) frameIndex = 4;
               if (randomNumber < 98) frameIndex = 3;
               if (randomNumber < 97) frameIndex = 2;
               if (randomNumber < 95) frameIndex = 1;
               if (randomNumber < 93) frameIndex = 0;
               if (randomNumber < 91) frameIndex = 8;
               if (randomNumber < 70) frameIndex = 7;
               if (randomNumber < 50) frameIndex = 6;

               outputGrid->set(x, y, new Tile(frameIndex));
           }
        });

        this->planeGrid = finalGrid;
        this->finishPlane();
    }



    PrimIsoGrid<bool>* PlaneGenerator::smooth(PrimIsoGrid<bool> *grid) {
        auto nextGrid = new PrimIsoGrid<bool>();
        nextGrid->initialize(grid->width, grid->height);
        grid->forEachCoord([this, nextGrid, grid](int x, int y) {
            auto currentVal = grid->get(x, y);
            bool nextVal = currentVal;
            int neighbors = this->getNeighborCount(grid, x, y);
            if (currentVal) {
                if (neighbors < 4) nextVal = false;
            } else {
                if (neighbors >= 6) nextVal = true;
            }
            nextGrid->set(x, y, nextVal);
        });

        delete grid;
        return nextGrid;
    }

    int PlaneGenerator::getNeighborCount(DsprGameServer::PrimIsoGrid<bool> *grid, int x, int y) {
        int output = 0;

        if(grid->get(x+2,y)) output+=1;
        if(grid->get(x-2,y)) output+=1;
        if(grid->get(x,y+2)) output+=1;
        if(grid->get(x,y-2)) output+=1;
        if(grid->get(x+1,y+1)) output+=1;
        if(grid->get(x-1,y-1)) output+=1;
        if(grid->get(x+1,y-1)) output+=1;
        if(grid->get(x-1,y+1)) output+=1;

        return output;
    }

    void PlaneGenerator::finishPlane() {
        this->playerStart = insulatePoint(findPlayerStart(), 10);
        this->riftLocation = insulatePoint(PlaneGenerator::getFurthestPointFromPoint(this->playerStart), 10);

        auto currentFeatureSet = std::unordered_set<Point>();
        currentFeatureSet.insert(this->playerStart);
        currentFeatureSet.insert(this->riftLocation);

        //create starting manafount
        {
            auto newManafountPoint = insulatePoint(PlaneGenerator::getPointSomeDisFromPoint(this->playerStart, 15), 10);
            this->manafountSet.insert(newManafountPoint);
            currentFeatureSet.insert(newManafountPoint);
        }

        //create "wild" manafounts
        for(int i=0;i<4;i++)
        {
            auto newManafountPoint = insulatePoint(PlaneGenerator::getFurthestPointFromPointSet(currentFeatureSet, 0), 10);
            this->manafountSet.insert(newManafountPoint);
            currentFeatureSet.insert(newManafountPoint);
        }

    }

    Point PlaneGenerator::findPlayerStart() {
        Point middlePoint = Point(this->planeGrid->width/2,this->planeGrid->height/2);

        //finding nearest tile to middle
        auto findCircle = CircleCache::get().getCircle(128);
        for(auto circleCoord : findCircle->coordList){
            int fx = middlePoint.x + circleCoord->x;
            int fy = middlePoint.y + circleCoord->y;
            if (this->planeGrid->get(fx, fy))
            {
                middlePoint.x = fx;
                middlePoint.y = fy;
                break;
            }
        }

        return PlaneGenerator::getFurthestPointFromPoint(middlePoint);
    }

    Point PlaneGenerator::getPointSomeDisFromPoint(Point& point, int distance) {
        auto newSet = std::unordered_set<Point>();
        newSet.insert(point);
        return getFurthestPointFromPointSet(newSet, distance);
    }

    Point PlaneGenerator::getFurthestPointFromPoint(Point& point) {
        auto newSet = std::unordered_set<Point>();
        newSet.insert(point);
        return getFurthestPointFromPointSet(newSet);
    }

    Point PlaneGenerator::getFurthestPointFromPointSet(std::unordered_set<Point> &pointSet, int limit) {

        auto closedMap = new std::unordered_map<int, PathNode*>();
        auto openHeap = new std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>();
        auto openMap = new std::unordered_map<int, PathNode*>();
        auto nodes = new std::list<PathNode*>();

        //put first node into openHeap/Map
        for (auto startingPoint : pointSet) {
            auto startNode = new PathNode(startingPoint.x, startingPoint.y, nullptr, 0);
            openHeap->push(startNode);
            openMap->emplace(startNode->getId(), startNode);
            nodes->push_back(startNode);
        }
        int lastPlacedId;

        while(openMap->size() > 0)
        {
            PathNode* currentNode = openHeap->top(); /*and then delete it*/ openHeap->pop();
            openMap->erase(currentNode->getId());
            lastPlacedId = currentNode->getId();

            if (limit > 0)
                if (currentNode->g >= limit) {
                    closedMap->emplace(currentNode->getId(), currentNode);
                    break;
                }

            //flood outwards
            std::list<PathNode*>* neighbors = getNeighbors(currentNode);
            for(auto newNode : *neighbors)
            {
                if (closedMap->count(newNode->getId()) != 0)
                {
                    delete newNode;
                    continue;
                }

                if (openMap->count(newNode->getId()) == 0)
                {
                    openHeap->push(newNode);
                    openMap->emplace(newNode->getId(), newNode);
                    nodes->push_back(newNode);
                }
                else
                {
                    delete newNode;
                    continue;
                }
            }

            //clean up neighbor list
            delete neighbors;

            closedMap->emplace(currentNode->getId(), currentNode);
        }

        auto lastTile = closedMap->at(lastPlacedId);
        Point output = Point(lastTile->x, lastTile->y);
        cleanUp(nodes, closedMap, openHeap, openMap);
        return output;
    }

    std::list<PathNode *> * PlaneGenerator::getNeighbors(PathNode *parent)
    {
        auto neighbors = new std::list<PathNode*>();

        tryAddNeighbor(neighbors, parent, 2, 0, 1);
        tryAddNeighbor(neighbors, parent, -2, 0, 1);
        tryAddNeighbor(neighbors, parent, 0, 2, 1);
        tryAddNeighbor(neighbors, parent, 0, -2, 1);

        tryAddNeighbor(neighbors, parent, 1, 1, 1);
        tryAddNeighbor(neighbors, parent, -1, -1, 1);
        tryAddNeighbor(neighbors, parent, 1, -1, 1);
        tryAddNeighbor(neighbors, parent, -1, 1, 1);

        return neighbors;
    }

    void PlaneGenerator::tryAddNeighbor(std::list<PathNode *> *neighborList, PathNode *parent, int xAdj, int yAdj, float cost)
    {

        int x = parent->x + xAdj;
        int y = parent->y + yAdj;
        auto tile = this->planeGrid->get(x, y);
        if (!tile) return;
        //if (this->game->unitManager->getEndPosAtCoord(x, y)) return;

        auto newPathNode = new PathNode(x, y, parent, cost);
        neighborList->push_front(newPathNode);
    };

    void PlaneGenerator::cleanUp(std::list<PathNode *> *nodes,
                                   std::unordered_map<int, PathNode*>* map,
                                   std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator>* heap,
                                   std::unordered_map<int, PathNode*>* map2) {
        for (auto node : *nodes)
        {
            delete node;
        }
        delete nodes;

        delete map;
        delete map2;
        delete heap;
    }

    Point PlaneGenerator::insulatePoint(Point point, int insulation) {
        auto findCircle = CircleCache::get().getCircle(insulation);
        for(auto circleCoord : findCircle->coordList){
            int fx = point.x + circleCoord->x;
            int fy = point.y + circleCoord->y;
            if (!this->planeGrid->get(fx, fy))
            {
                int newx = point.x - MathUtils::SignOrZero(circleCoord->x);
                int newy = point.y - MathUtils::SignOrZero(circleCoord->y);
                if (this->planeGrid->get(newx,newy))
                {
                    point.x = newx; point.y = newy;
                }
            }
        }

        return point;
    }
}