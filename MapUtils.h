#pragma once

//
// Created by connor on 7/23/18.
//

class MapUtils { ;
public:
    template<typename K, typename V>
    static bool mapContains(const std::map<K, V>& myMap, K key){
        auto it = myMap.find(key);
        if(it != myMap.end())
            return true;
        return false;
    }
};