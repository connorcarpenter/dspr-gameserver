//
// Created by connor on 9/1/18.
//

#include "IsoBoxCache.h"

namespace DsprGameServer
{
    IsoBoxCache::IsoBoxCache() {}

    IsoBoxCache::~IsoBoxCache()
    {
        for(const auto& isoBoxPair : isoBoxMap)
        {
            auto isobox = isoBoxPair.second;
            delete isobox;
        }
    }

    IsoBox* IsoBoxCache::getIsoBox(int width, int height)
    {
        int isoBoxHandle = (width * 1000) + height;
        if (this->isoBoxMap.count(isoBoxHandle) > 0)
            return this->isoBoxMap.at(isoBoxHandle);
        auto newCircle = new IsoBox(width, height);
        this->isoBoxMap.emplace(isoBoxHandle, newCircle);
        return newCircle;
    }
}