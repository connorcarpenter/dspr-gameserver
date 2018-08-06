#pragma once;

//
// Created by connor on 7/23/18.
//

namespace DsprGameServer {
    class Tile {
    public:
        Tile(int frame);

        int frame;
        bool walkable = true;
    };
}