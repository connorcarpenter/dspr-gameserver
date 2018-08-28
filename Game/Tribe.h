//
// Created by connor on 8/27/18.
//

#include <set>

namespace DsprGameServer
{
    class Tribe
    {
    public:
        void setEnemy(Tribe* otherTribe);
    private:
        std::set<Tribe*> enemies;
    };
}
