//
// Created by connor on 8/29/18.
//

#include <string>

namespace DsprGameServer{

    class Int {
    public:
        void Set(int newInt);
        int Get();
        std::string serialize();
    private:
        int currentInt;
    };
}