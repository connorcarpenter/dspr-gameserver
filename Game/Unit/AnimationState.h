//
// Created by connor on 8/29/18.
//

#include <string>

namespace DsprGameServer{
    enum AnimationStates { Walking, Attacking };
    class AnimationState {
    public:
        void SetState(AnimationStates newState);
        AnimationStates GetState();

        void SetHeading(int newHeading);
        int GetHeading();

        std::string serialize();
    private:
        AnimationStates currentState = Walking;
        int currentHeading = 0;
    };
}