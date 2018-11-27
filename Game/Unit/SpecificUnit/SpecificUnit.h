#pragma once

//
// Created by connor on 11/2/18.
//

namespace DsprGameServer {
    class SpecificUnit {
    public:
        virtual void update() {};
        virtual void specialAction(int actionIndex) {};
        virtual void specialAction(int actionIndex, int x, int y) {};
    };
}