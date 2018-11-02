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
        explicit Synced(const std::string& name, T* newObj, bool startsDirty = false)
        {
            this->internal = newObj;
            this->varName = name;
            this->dataIsDirty = startsDirty;
        }

        ~Synced()
        {
            delete internal;
        }

        T* obj() {
            return this->internal;
        }

        T* dirtyObj() {
            this->dirty();
            return this->internal;
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
            newStr.append(internal->serialize());
            return newStr;
        }

    private:
        T* internal;
        bool dataIsDirty = false;
        std::string varName;
    };
}
