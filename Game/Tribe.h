//
// Created by connor on 8/27/18.
//

#include <set>

namespace DsprGameServer
{
    class Tribe
    {
    public:
        Tribe(int color);
        void setEnemy(Tribe* otherTribe);
        int index = -1;
    private:
        std::set<Tribe*> enemies;
    };
}
