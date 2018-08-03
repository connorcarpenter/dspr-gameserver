#include <uWS/uWS.h>
#include <iostream>
#include "Game.h"
#include "StringUtils.h"
#include "MapUtils.h"
#include "GameServer.h"

using namespace DsprGameServer;

int main()
{
    //game setup


    //network stuff

    uWS::Hub h;

    h.onConnection([&h](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
    {
        ws->send("auth/1.0/gametoken|\r\n", uWS::BINARY);//the \r\n makes it happen on EMSC! Don't remove!
        std::cout << "dspr-gameserver: Received 'onConnection', Sent 'auth/1.0/gametoken|'" << std::endl;
    });

    h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length)
    {
        std::cout << "dspr-gameserver: disconnection" << std::endl;
    });

    h.onError([](void *user) {
        std::cout << "dspr-gameserver: error" << std::endl;
    });

    h.onMessage([&h](uWS::WebSocket<uWS::SERVER> *ws, char *data, size_t length, uWS::OpCode opCode)
    {
        std::string msgString = std::string(data, length);

        if (StringUtils::endsWith(msgString,"\r"))
            msgString = msgString.substr(0, msgString.length()-1);
        if (StringUtils::endsWith(msgString,"\n"))
            msgString = msgString.substr(0, msgString.length()-1);

        std::vector <std::string> parts = StringUtils::split(msgString, '|');

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
            game->addPlayer(playerToken, ws);
        }
        else if (parts.at(0).compare("unit/1.0/order") == 0)
        {
            auto unitIdListStrings = StringUtils::split(parts[2], ',');
            auto orderStrings = StringUtils::split(parts[3], ',');

            std::list<int> unitIdList;
            for (const auto &str : unitIdListStrings){
                unitIdList.push_front(stoi(str));
            }

            int x = stoi(orderStrings[0]);
            int y = stoi(orderStrings[1]);

            game->receiveUnitOrder(unitIdList, x, y);

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
