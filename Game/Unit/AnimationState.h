#pragma once

//
// Created by connor on 8/29/18.
//

#include <string>

namespace DsprGameServer{
    enum AnimationStates { Walking, Attacking, Gathering, AnimationStatesMaxValue };
    class AnimationState {
    public:
        void SetState(AnimationStates newState);
        AnimationStates GetState();

        void SetHeading(int newHeading);
        int GetHeading();
        void SetStateChangeFunction(std::function<void()> func);

        std::string serialize();

    private:
        AnimationStates currentState = Walking;
        int currentHeading = 0;
        std::function<void()> stateChangeFunction = nullptr;
    };
}