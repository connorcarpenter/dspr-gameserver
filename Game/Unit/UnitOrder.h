#pragma once

//
// Created by connor on 9/10/18.
//

namespace DsprGameServer {
    enum UnitOrder {
        Move,
        AttackMove,
        Follow,
        AttackTarget,
        Stop,
        Hold,
        Train,
        Gather,
        Pickup
    };
}