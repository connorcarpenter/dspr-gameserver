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

namespace DsprGameServer
{
    class MessageReceiver{


    public:
        MessageReceiver() = default;

        void receive(DsprGameServer::PlayerData *playerData, std::string bffToken, std::string msgString)
        {
            std::vector <std::string> parts = StringUtils::split(msgString, '|');

            // Add new player
            if (parts.at(0).compare("player/1.0/add") == 0)
            {
                if (parts.at(1).compare(bffToken) == 0)
                {
                    GameServer::get().addPlayerToken(parts.at(2));
                    std::cout << "dspr-gameserver: Received new player" << std::endl;
                }
                return;
            }

            std::string playerToken = parts.at(1);

            // Validate the playertoken
            if (!MapUtils::mapContains(GameServer::get().playerCodeToGameMap, playerToken))
            {
                std::cout << "dspr-gameserver: Received invalid playertoken: " << playerToken << std::endl;
                return;
            }

            // Get the game associated with the token
            Game* game = GameServer::get().playerCodeToGameMap.at(playerToken);

            // Process message
            if (parts.at(0).compare("auth/1.0/gametoken") == 0)
            {
                std::cout << "dspr-gameserver: Received 'auth/1.0/gametoken', added player to game" << std::endl;
                game->addPlayer(playerToken, playerData);
            }
            else if (parts.at(0).compare("unit/1.0/order") == 0)
            {
                auto unitIdListStrings = StringUtils::split(parts[2], ',');
                auto orderStrings = StringUtils::split(parts[3], ',');

                std::list<int> unitIdList;
                for (const auto &str : unitIdListStrings){
                    unitIdList.push_front(stoi(str));
                }

                UnitOrder orderIndex = static_cast<UnitOrder>(stoi(orderStrings[0]));

                switch (orderIndex)
                {
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
                        game->unitManager->receiveItemSwapOrder(unitIdList.front(), beforeSlotIndex, afterSlotIndex);
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
                }

                std::cout << "dspr-gameserver: Received '" << msgString << "'" << std::endl;
            }
            else if (parts.at(0).compare("chat/1.0/send") == 0)
            {
                game->chatManager->receiveMessage(parts[2], playerData);
                std::cout << "dspr-gameserver: Received '" << msgString << "'" << std::endl;
            }
        }
    };
}