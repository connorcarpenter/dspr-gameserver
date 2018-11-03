//
// Created by connor on 8/29/18.
//

#include <functional>
#include "AnimationState.h"

namespace DsprGameServer
{
    void AnimationState::SetState(DsprGameServer::AnimationStates newState) {
        if (this->stateChangeFunction != nullptr) {
            if (newState != this->currentState) {
                this->stateChangeFunction();
            }
        }
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

    void AnimationState::SetStateChangeFunction(std::function<void()> func) {
        this->stateChangeFunction = func;
    }
}

