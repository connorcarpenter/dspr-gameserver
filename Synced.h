#pragma once

//
// Created by connor on 8/2/18.
//

#include <string>

namespace DsprGameServer
{
    template<class T>
    class Synced
    {
    public:
        explicit Synced(const std::string& name, T* newObj)
        {
            this->obj = newObj;
            this->varName = name;
        }

        T * get() const {
            return this->obj;
        }



        void dirty() {
            this->dataIsDirty = true;
        }

        void clean() {
            this->dataIsDirty = false;
        }

        bool isDirty() {
            return this->dataIsDirty;
        }

        std::string serialize(){
            std::string newStr;
            newStr.append(varName);
            newStr.append(":");
            newStr.append(obj->serialize());
            return newStr;
        }

    private:
        T* obj;
        bool dataIsDirty = false;
        std::string varName;
    };
}
