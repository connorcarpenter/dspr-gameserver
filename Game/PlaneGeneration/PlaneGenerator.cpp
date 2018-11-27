//
// Created by connor on 11/27/18.
//

#include "PlaneGenerator.h"
#include "../../PrimIsoGrid.h"
#include "../../Math/MathUtils.h"

namespace DsprGameServer {

    void PlaneGenerator::fill(DsprGameServer::PtrIsoGrid<DsprGameServer::Tile *> *outputGrid)
    {
        //initial randomization
        auto finalGrid = new PrimIsoGrid<bool>();
        finalGrid->initialize(outputGrid->width, outputGrid->height);

        finalGrid->forEachCoord([finalGrid](int x, int y){
           bool val = (MathUtils::getRandom(100) <= 55);
            finalGrid->set(x,y,val);
        });

//        //smoothing
//        for(int i=0;i<5;i++) {
//            auto nextGrid = new PrimIsoGrid<bool>();
//            nextGrid->initialize(outputGrid->width, outputGrid->height);
//            outputGrid->forEachCoord([this, nextGrid, finalGrid](int x, int y) {
//                auto currentVal = finalGrid->get(x, y);
//                bool nextVal = currentVal;
//                int neighbors = this->getNeighbors(finalGrid, x, y);
//                if (currentVal) {
//                    if (neighbors < 4) nextVal = false;
//                } else {
//                    if (neighbors >= 6) nextVal = true;
//                }
//                nextGrid->set(x, y, nextVal);
//            });
//
//            delete finalGrid;
//            finalGrid = nextGrid;
//        }

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

        delete finalGrid;
        int i = 1;
        i++;
    }

    int PlaneGenerator::getNeighbors(DsprGameServer::PrimIsoGrid<bool> *grid, int x, int y) {
        int output = 0;
        for (int i=-1;i<2;i++)
            for(int j=-1;j<2;j++)
            {
                if (j==0 && i==0)continue;
                if(grid->get(x+i,y+j)) output+=1;
            }

        return output;
    }
}