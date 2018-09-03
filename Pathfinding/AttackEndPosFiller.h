//
// Created by connor on 9/2/18.
//

#include "../Game/Game.h"

namespace DsprGameServer {
    class AttackEndPosFiller {

    public:
        AttackEndPosFiller(DsprGameServer::Game *game);

        void fillEndTiles(std::shared_ptr<DsprGameServer::Path> path, int unitNumber);
    };
}