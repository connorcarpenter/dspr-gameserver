//
// Created by connor on 10/26/18.
//

#include "ConstructionQueue.h"
#include "Unit.h"

namespace DsprGameServer
{
    ConstructionQueue::ConstructionQueue(Unit *masterUnit) {
        this->masterUnit = masterUnit;
    }

    bool ConstructionQueue::atMaxQueue()
    {
        return this->unitQueue.size() >= 7;
    }

    void ConstructionQueue::enqueue(UnitTemplate *unitTemplate)
    {
        this->unitQueue.push(unitTemplate);
        this->dirtyQueue = true;
    }

    void ConstructionQueue::update()
    {
        if (this->unitQueue.size() > 0)
        {
            this->currentBuildTime += 1;

            auto currentlyBuildingUnit = unitQueue.front();
            if (this->currentBuildTime >= currentlyBuildingUnit->buildTime)
            {
                this->currentBuildTime = 0;
                this->updateTime = 0;
                unitQueue.pop();
                this->masterUnit->finishTraining(currentlyBuildingUnit);
                this->dirtyQueue = true;
                this->dirtyTime = true;
            }

            this->updateTime += 1;
            if (this->updateTime > currentlyBuildingUnit->buildTime/4)
            {
                this->dirtyTime = true;
                this->updateTime = 0;
            }
        }
    }

    bool ConstructionQueue::isDirty() {
        return this->dirtyQueue || this->dirtyTime;
    }

    std::string ConstructionQueue::getUpdate(bool overrideDirty) {
        bool first = true;
        std::string newStr;
        newStr.append("cq:");
        if (this->dirtyTime || overrideDirty){
            newStr.append("bt-");
            newStr.append(std::to_string(this->currentBuildTime));
            first = false;
        }
        if (this->dirtyQueue || overrideDirty) {
            if (!first) newStr.append("+");
            newStr.append("q-");

            first = true;
            auto copiedQueue = std::queue<UnitTemplate *>(this->unitQueue);
            while (copiedQueue.size() > 0) {
                auto front = copiedQueue.front();
                if (front!=nullptr) {

                    if (first) {first = false;}
                    else {
                        newStr.append(",");
                    }
                    newStr.append(std::to_string(front->index));
                }
                copiedQueue.pop();
            }
        }

        return newStr;
    }

    void ConstructionQueue::clean() {
        this->dirtyTime = false;
        this->dirtyQueue = false;
    }

    DsprMessage::ConstructionQueueMsgV1 * ConstructionQueue::serialize() {
        DsprMessage::ConstructionQueueMsgV1* msg = new DsprMessage::ConstructionQueueMsgV1();
        msg->buildTime.set(this->currentBuildTime);
        msg->queue.initBytes(this->unitQueue.size());

        int i = 0;
        auto copiedQueue = std::queue<UnitTemplate *>(this->unitQueue);
        while (copiedQueue.size() > 0) {
            auto front = copiedQueue.front();
            if (front!=nullptr) {
                msg->queue.setArray(i, front->index);
            }
            copiedQueue.pop();
            i++;
        }

        return msg;
    }
}