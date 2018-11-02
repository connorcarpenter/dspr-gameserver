#pragma once

//
// Created by connor on 11/2/18.
//

#include <map>
#include "../Math/Int.h"
#include "Unit/Synced.h"
#include "../PlayerData.h"

namespace DsprGameServer {
    class EconomyManager {
    public:

        EconomyManager(Game *pGame);

        ~EconomyManager();
        int getPop(Tribe* tribe);
        int getPopMax(Tribe* tribe);
        int getMana(Tribe* tribe);

        void setPop(Tribe* tribe, int value);
        void setPopMax(Tribe* tribe, int value);
        void setMana(Tribe* tribe, int value);

        void addTribe(Tribe *tribe);
        void removeTribe(Tribe *tribe);

        void sendUpdates(PlayerData *player, bool overrideDirty);

        void clean();

        void setManaClean(Tribe *pTribe, int value);

    private:

        std::map<Tribe*, Synced<Int>*> playerToPopMap;
        std::map<Tribe*, Synced<Int>*> playerToPopMaxMap;
        std::map<Tribe*, Synced<Int>*> playerToManaMap;

        bool anyVarIsDirty(Tribe *tribe);

        Game *game = nullptr;
    };
}
