#pragma once

//
// Created by connor on 11/12/18.
//

#include <iostream>
#include "PlayerData.h"
#include "StringUtils.h"
#include "GameServer.h"
#include "MapUtils.h"
#include "Game/Unit/UnitOrder.h"
#include "Game/Unit/UnitManager.h"
#include "Game/ChatManager.h"
#include "DsprMessage/ToServerMsg.h"

namespace DsprGameServer
{
    class MessageReceiver{

    public:
        MessageReceiver() = default;

        std::string bffString = std::basic_string<char>("player/1.0/add");

        void receive(DsprGameServer::PlayerData *playerData, std::string bffToken, char *data, int length)
        {
            bool isFromBff = true;
            for(int i=0;(i<length && i<bffString.length());i++)
            {
                if (data[i] != bffString.at(i))
                {
                    isFromBff = false;
                    break;
                }
            }
            if (isFromBff)
            {
                std::string msgString = std::basic_string<char>(data, length);
                if (StringUtils::endsWith(msgString,"\r"))
                    msgString = msgString.substr(0, msgString.length()-1);
                if (StringUtils::endsWith(msgString,"\n"))
                    msgString = msgString.substr(0, msgString.length()-1);
                std::vector<std::string> parts = StringUtils::split(msgString, '|');

                // Add new player
                if (parts.at(0).compare("player/1.0/add") == 0) {
                    if (parts.at(1).compare(bffToken) == 0) {
                        GameServer::get().addPlayerToken(parts.at(2));
                        std::cout << "dspr-gameserver: Received new player w/ token: " << parts.at(2) << std::endl;
                    }
                    return;
                }
            }

            std::shared_ptr<DsprMessage::CStr> dataCStr = DsprMessage::CStr::make_cstr((unsigned char*) data, length);
            DsprMessage::ToServerMsg toServerMsg = DsprMessage::ToServerMsg(dataCStr);

            Game* game = nullptr;
            std::shared_ptr<std::string> playerTokenPtr = nullptr;
            if (toServerMsg.msgType.get() != DsprMessage::ToServerMsg::MessageType::StandardMessage)
            {
                playerTokenPtr = toServerMsg.authToken.toStdString();
                std::string playerTokenStr = *playerTokenPtr.get();

                // Validate the playertoken
                if (!MapUtils::mapContains(GameServer::get().playerCodeToGameMap, playerTokenStr)) {
                    std::cout << "dspr-gameserver: Received invalid playertoken: " << playerTokenPtr << std::endl;
                    return;
                }

                // Get the game associated with the token
                if (GameServer::get().playerCodeToGameMap.count(playerTokenStr) <=0)
                    return;
                game = GameServer::get().playerCodeToGameMap.at(playerTokenStr);
            }

            switch(toServerMsg.msgType.get())
            {
                case DsprMessage::ToServerMsg::MessageType::StartGame:
                {
                    std::cout << "dspr-gameserver: Received 'auth/1.0/gametoken', added player to game" << std::endl;
                    game->addPlayer(*playerTokenPtr.get(), playerData);
                }
                    break;
                case DsprMessage::ToServerMsg::MessageType::ChatSend:
                {
                    DsprMessage::ChatSendServerMsgV1 chatSendServerMsgV1(toServerMsg.msgBytes);
                    auto chatStr = chatSendServerMsgV1.chatMsg.toStdString();
                    game->chatManager->receiveMessage(*chatStr, playerData);
                    std::cout << "dspr-gameserver: Received Chat Msg:'" << (*chatStr) << "'" << std::endl;
                }
                    break;
                case DsprMessage::ToServerMsg::MessageType::UnitOrder:
                {
//                    ///TESTING
//                    std::shared_ptr<DsprMessage::CStr> testCstr = DsprMessage::CStr::make_cstr(toServerMsg.msgBytes);
//                    DsprMessage::CStr* testCstrR = testCstr.get();
//                    //TESTING

                    DsprMessage::UnitOrderMsgV1 unitOrderMsgV1(toServerMsg.msgBytes);

                    std::list<int> unitIdList;
                    for (int i = 0;i<unitOrderMsgV1.unitIds.size();i++) {
                        unitIdList.push_front(unitOrderMsgV1.unitIds.get(i));
                    }

                    UnitOrder orderIndex = static_cast<UnitOrder>(unitOrderMsgV1.orderIndex.get());

                    switch (orderIndex) {
                        case Move: {
                            int x = unitOrderMsgV1.otherNumbers.get(0);
                            int y = unitOrderMsgV1.otherNumbers.get(1);
                            game->unitManager->receiveMoveOrder(unitIdList, x, y);
                        }
                            break;
                        case Follow: {
                            int targetUnitId = unitOrderMsgV1.otherNumbers.get(0);
                            game->unitManager->receiveFollowOrder(unitIdList, targetUnitId);
                        }
                            break;
                        case AttackTarget: {
                            int targetUnitId = unitOrderMsgV1.otherNumbers.get(0);
                            game->unitManager->receiveAttackTargetOrder(unitIdList, targetUnitId);
                        }
                            break;
                        case AttackMove: {
                            int x = unitOrderMsgV1.otherNumbers.get(0);
                            int y = unitOrderMsgV1.otherNumbers.get(1);
                            game->unitManager->receiveAttackMoveOrder(unitIdList, x, y);
                        }
                            break;
                        case Stop: {
                            game->unitManager->receiveStopOrder(unitIdList);
                        }
                            break;
                        case Hold: {
                            game->unitManager->receiveHoldOrder(unitIdList);
                        }
                            break;
                        case Train: {
                            int unitTemplateIndex = unitOrderMsgV1.otherNumbers.get(0);
                            game->unitManager->receiveTrainOrder(unitIdList, unitTemplateIndex);
                        }
                            break;
                        case Gather: {
                            int targetUnitId = unitOrderMsgV1.otherNumbers.get(0);
                            game->unitManager->receiveGatherOrder(unitIdList, targetUnitId);
                        }
                            break;
                        case Pickup: {
                            int targetItemId = unitOrderMsgV1.otherNumbers.get(0);
                            game->unitManager->receivePickupOrder(unitIdList, targetItemId);
                        }
                            break;
                        case ItemSwap: {
                            int beforeSlotIndex = unitOrderMsgV1.otherNumbers.get(0);
                            int afterSlotIndex = unitOrderMsgV1.otherNumbers.get(1);
                            game->unitManager->receiveItemSwapOrder(unitIdList.front(), beforeSlotIndex,
                                                                    afterSlotIndex);
                        }
                            break;
                        case ItemDrop: {
                            int slotIndex = unitOrderMsgV1.otherNumbers.get(0);
                            int x = unitOrderMsgV1.otherNumbers.get(1);
                            int y = unitOrderMsgV1.otherNumbers.get(2);
                            game->unitManager->receiveItemDropOrder(unitIdList.front(), slotIndex, x, y);
                        }
                            break;
                        case ItemGive: {
                            int slotIndex = unitOrderMsgV1.otherNumbers.get(0);
                            int targetUnitId = unitOrderMsgV1.otherNumbers.get(1);
                            game->unitManager->receiveItemGiveOrder(unitIdList.front(), slotIndex, targetUnitId);
                        }
                            break;
                        case RallyPoint: {
                            int x = unitOrderMsgV1.otherNumbers.get(0);
                            int y = unitOrderMsgV1.otherNumbers.get(1);
                            game->unitManager->receiveRallyPointOrder(unitIdList, x, y);
                        }
                            break;
                        case RallyUnit: {
                            int targetUnitId = unitOrderMsgV1.otherNumbers.get(0);
                            game->unitManager->receiveRallyUnitOrder(unitIdList, targetUnitId);
                        }
                            break;
                        default:
                        {
                            int i = 12; //:(
                        }
                        break;
                    }
                }
                    break;
                case DsprMessage::ToServerMsg::MessageType::StandardMessage:
                {
                    std::shared_ptr<std::string> msgString = toServerMsg.msgBytes.toStdString();

                    std::vector<std::string> parts = StringUtils::split(*msgString.get(), '|');

                    std::string playerToken = parts.at(1);

                    // Validate the playertoken
                    if (!MapUtils::mapContains(GameServer::get().playerCodeToGameMap, playerToken)) {
                        std::cout << "dspr-gameserver: Received invalid playertoken: " << playerToken << std::endl;
                        return;
                    }

                    // Get the game associated with the token
                    if (GameServer::get().playerCodeToGameMap.count(playerToken) <=0)
                        return;
                    game = GameServer::get().playerCodeToGameMap.at(playerToken);

                    // Process message
                    if (parts.at(0).compare("unit/1.0/order") == 0) {
                        auto unitIdListStrings = StringUtils::split(parts[2], ',');
                        auto orderStrings = StringUtils::split(parts[3], ',');

                        std::list<int> unitIdList;
                        for (const auto &str : unitIdListStrings) {
                            unitIdList.push_front(stoi(str));
                        }

                        UnitOrder orderIndex = static_cast<UnitOrder>(stoi(orderStrings[0]));

                        switch (orderIndex) {
                            case Move: {
                                int x = stoi(orderStrings[1]);
                                int y = stoi(orderStrings[2]);
                                game->unitManager->receiveMoveOrder(unitIdList, x, y);
                            }
                                break;
                            case Follow: {
                                int targetUnitId = stoi(orderStrings[1]);
                                game->unitManager->receiveFollowOrder(unitIdList, targetUnitId);
                            }
                                break;
                            case AttackTarget: {
                                int targetUnitId = stoi(orderStrings[1]);
                                game->unitManager->receiveAttackTargetOrder(unitIdList, targetUnitId);
                            }
                                break;
                            case AttackMove: {
                                int x = stoi(orderStrings[1]);
                                int y = stoi(orderStrings[2]);
                                game->unitManager->receiveAttackMoveOrder(unitIdList, x, y);
                            }
                                break;
                            case Stop: {
                                game->unitManager->receiveStopOrder(unitIdList);
                            }
                                break;
                            case Hold: {
                                game->unitManager->receiveHoldOrder(unitIdList);
                            }
                                break;
                            case Train: {
                                int unitTemplateIndex = stoi(orderStrings[1]);
                                game->unitManager->receiveTrainOrder(unitIdList, unitTemplateIndex);
                            }
                                break;
                            case Gather: {
                                int targetUnitId = stoi(orderStrings[1]);
                                game->unitManager->receiveGatherOrder(unitIdList, targetUnitId);
                            }
                                break;
                            case Pickup: {
                                int targetItemId = stoi(orderStrings[1]);
                                game->unitManager->receivePickupOrder(unitIdList, targetItemId);
                            }
                                break;
                            case ItemSwap: {
                                int beforeSlotIndex = stoi(orderStrings[1]);
                                int afterSlotIndex = stoi(orderStrings[2]);
                                game->unitManager->receiveItemSwapOrder(unitIdList.front(), beforeSlotIndex,
                                                                        afterSlotIndex);
                            }
                                break;
                            case ItemDrop: {
                                int slotIndex = stoi(orderStrings[1]);
                                int x = stoi(orderStrings[2]);
                                int y = stoi(orderStrings[3]);
                                game->unitManager->receiveItemDropOrder(unitIdList.front(), slotIndex, x, y);
                            }
                                break;
                            case ItemGive: {
                                int slotIndex = stoi(orderStrings[1]);
                                int targetUnitId = stoi(orderStrings[2]);
                                game->unitManager->receiveItemGiveOrder(unitIdList.front(), slotIndex, targetUnitId);
                            }
                                break;
                            case RallyPoint: {
                                int x = stoi(orderStrings[1]);
                                int y = stoi(orderStrings[2]);
                                game->unitManager->receiveRallyPointOrder(unitIdList, x, y);
                            }
                                break;
                            case RallyUnit: {
                                int targetUnitId = stoi(orderStrings[1]);
                                game->unitManager->receiveRallyUnitOrder(unitIdList, targetUnitId);
                            }
                                break;
                        }

                        std::cout << "dspr-gameserver: Received '" << msgString << "'" << std::endl;
                    }
                }
                    break;
                default:
                    int i = 12; //:(
                    break;
            }
        }
    };
}