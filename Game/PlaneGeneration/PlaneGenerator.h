//
// Created by connor on 11/27/18.
//

#include "../../PtrIsoGrid.h"
#include "../Tile.h"
#include "../../PrimIsoGrid.h"

namespace DsprGameServer {
    class PlaneGenerator {

    public:
        void fill(DsprGameServer::PtrIsoGrid<DsprGameServer::Tile*>* outputGrid);

        int getNeighbors(DsprGameServer::PrimIsoGrid<bool> *grid, int x, int y);
    };
}