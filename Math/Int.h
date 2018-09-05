//
// Created by connor on 8/29/18.
//

#include <string>

namespace DsprGameServer{

    class Int {
    public:
        Int(int newValue);
        void Set(int newInt);
        int Get();
        void Subtract(int subInt);
        std::string serialize();
    private:
        int value;
    };
}