#pragma once

//
// Created by connor on 9/1/18.
//

#include "IsoBox.h"
#include <map>

namespace DsprGameServer
{
    class IsoBoxCache
    {
    public:
        static IsoBoxCache& get() {
            static IsoBoxCache isoBoxCache;
            return isoBoxCache;
        }
        ~IsoBoxCache();
        IsoBox* getIsoBox(int width, int height);
    private:

        IsoBoxCache();

        std::map<int, IsoBox*> isoBoxMap;
    };
}