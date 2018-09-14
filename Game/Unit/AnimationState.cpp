//
// Created by connor on 8/29/18.
//

#include "AnimationState.h"

namespace DsprGameServer
{
    void AnimationState::SetState(DsprGameServer::AnimationStates newState) {
        this->currentState = newState;
    }

    AnimationStates AnimationState::GetState() {
        return this->currentState;
    }

    void AnimationState::SetHeading(int newHeading) {
        this->currentHeading = newHeading;
    }

    int AnimationState::GetHeading() {
        return this->currentHeading;
    }

    std::string AnimationState::serialize() {
        std::string str;
        str.append(std::to_string((int) this->currentState));
        str.append(",");
        str.append(std::to_string(this->currentHeading));
        return str;
    }
}

