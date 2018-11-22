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
            if (toServerMsg.msgType.get() == DsprMessage::ToServerMsg::MessageType::StandardMessage)
            {
                char* typicalCharString = new char[toServerMsg.msgBytes.size()+1];
                for (int i=0;i<toServerMsg.msgBytes.size();i++)
                {
                    typicalCharString[i] = toServerMsg.msgBytes.get(i);
                }
                typicalCharString[toServerMsg.msgBytes.size()] = '\0';
                std::string msgString = std::basic_string<char>(typicalCharString, toServerMsg.msgBytes.size());
                if (StringUtils::endsWith(msgString,"\r"))
                    msgString = msgString.substr(0, msgString.length()-1);
                if (StringUtils::endsWith(msgString,"\n"))
                    msgString = msgString.substr(0, msgString.length()-1);
                std::vector<std::string> parts = StringUtils::split(msgString, '|');

                std::string playerToken = parts.at(1);

                // Validate the playertoken
                if (!MapUtils::mapContains(GameServer::get().playerCodeToGameMap, playerToken)) {
                    std::cout << "dspr-gameserver: Received invalid playertoken: " << playerToken << std::endl;
                    return;
                }

                // Get the game associated with the token
                Game *game = GameServer::get().playerCodeToGameMap.at(playerToken);

                // Process message
                if (parts.at(0).compare("auth/1.0/gametoken") == 0) {
                    std::cout << "dspr-gameserver: Received 'auth/1.0/gametoken', added player to game" << std::endl;
                    game->addPlayer(playerToken, playerData);
                } else if (parts.at(0).compare("unit/1.0/order") == 0) {
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
                } else if (parts.at(0).compare("chat/1.0/send") == 0) {
                    game->chatManager->receiveMessage(parts[2], playerData);
                    std::cout << "dspr-gameserver: Received '" << msgString << "'" << std::endl;
                }
            }
        }
    };
}