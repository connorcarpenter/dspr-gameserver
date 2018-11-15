#pragma once

//
// Created by connor on 8/2/18.
//

#include <string>
#include <functional>

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
            if (this->cleanFunc != nullptr) this->cleanFunc();
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

        void setCleanFunc(std::function<void()> cleanFunc) {
            this->cleanFunc = cleanFunc;
        }

    private:
        T* internal;
        bool dataIsDirty = false;
        std::string varName;
        std::function<void()> cleanFunc = nullptr;
    };
}
