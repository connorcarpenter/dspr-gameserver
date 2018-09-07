//
// Created by connor on 9/7/18.
//

#include "../PrimIsoGrid.h"
#include <map>

namespace DsprGameServer
{
    class Tribe;
    class Game;

    class FogManager
    {

    public:

        FogManager(Game *game);
        ~FogManager();

        void addTribe(Tribe* newTribe);
        void removeTribe(Tribe* tribe);

        void revealFog(Tribe* tribe, int x, int y, int radius);
        void conceilFog(Tribe* tribe, int x, int y, int radius);
        bool tileIsClear(Tribe* tribe, int x, int y);
        bool tileIsInFog(Tribe* tribe, int x, int y);
        bool tileIsInShroud(Tribe* tribe, int x, int y);

    private:

        std::map<Tribe*, PrimIsoGrid<int>*> fogGridMap;
        Game* game = nullptr;

        void updateFog(Tribe* tribe, int x, int y, int radius, bool reveal);
        int getFogAmount(Tribe* tribe, int x, int y);
    };
}