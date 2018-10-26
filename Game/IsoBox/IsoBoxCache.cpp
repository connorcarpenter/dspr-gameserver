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

        for(const auto& isoBoxPair : hollowIsoBoxMap)
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
        auto newIsoBox = new IsoBox(width, height, false);
        this->isoBoxMap.emplace(isoBoxHandle, newIsoBox);
        return newIsoBox;
    }

    IsoBox* IsoBoxCache::getHollowIsoBox(int width, int height)
    {
        int isoBoxHandle = (width * 1000) + height;
        if (this->hollowIsoBoxMap.count(isoBoxHandle) > 0)
            return this->hollowIsoBoxMap.at(isoBoxHandle);
        auto newIsoBox = new IsoBox(width, height, true);
        this->hollowIsoBoxMap.emplace(isoBoxHandle, newIsoBox);
        return newIsoBox;
    }
}