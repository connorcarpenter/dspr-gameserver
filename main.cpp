#include <uWS/uWS.h>
#include <iostream>
#include "Game/Game.h"
#include "StringUtils.h"
#include "MapUtils.h"
#include "GameServer.h"
#include "Game/Unit/UnitManager.h"
#include "PlayerData.h"
#include "Game/Unit/UnitOrder.h"

using namespace DsprGameServer;

int main()
{
    std::string bffToken = "nwlrbbmqbhcdacon";

    uWS::Hub h;

    h.onConnection([&h](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
    {
        PlayerData* playerData = new PlayerData(ws);
        ws->setUserData(playerData);

        ws->send("auth/1.0/gametoken|\r\n", uWS::BINARY);//the \r\n makes it happen on EMSC! Don't remove!
        std::cout << "dspr-gameserver: Received 'onConnection', Sent 'auth/1.0/gametoken|'" << std::endl;
    });

    h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length)
    {
        PlayerData* playerData = (PlayerData*) ws->getUserData();
        delete playerData;

        std::cout << "dspr-gameserver: disconnection" << std::endl;
    });

    h.onError([](void *user) {
        std::cout << "dspr-gameserver: error" << std::endl;
    });

    h.onMessage([&h, &bffToken](uWS::WebSocket<uWS::SERVER> *ws, char *data, size_t length, uWS::OpCode opCode)
    {
        PlayerData* playerData = (PlayerData*) ws->getUserData();

        std::string msgString = std::string(data, length);

        if (StringUtils::endsWith(msgString,"\r"))
            msgString = msgString.substr(0, msgString.length()-1);
        if (StringUtils::endsWith(msgString,"\n"))
            msgString = msgString.substr(0, msgString.length()-1);

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
            }

            std::cout << "dspr-gameserver: Received '" << msgString << "'" << std::endl;
        }
    });

    auto timer = new Timer(h.getLoop());
    timer->start([](Timer* timer){
        GameServer::get().step();
    }, GameServer::get().tickMs, GameServer::get().tickMs);

    h.getDefaultGroup<uWS::SERVER>().startAutoPing(30000);
    if (h.listen("localhost", 3173)) {
        std::cout << "dspr-gameserver: Listening to port 3173" << std::endl;
    } else {
        std::cerr << "dspr-gameserver: Failed to listen to port" << std::endl;
        return -1;
    }

    h.run();
}
