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
    std::map<std::string, Game*> gameMap;
    gameMap.insert(std::pair<std::string, Game*>("player1code", new Game()));

    //network stuff

    uWS::Hub h;

    h.onConnection([&h](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
    {
        ws->send("auth/1.0/gametoken|\r\n", uWS::BINARY);//the \r\n makes it happen on EMSC! Don't remove!
        std::cout << "dspr-gameserver: Received 'onConnection', Sent 'auth/1.0/gametoken|'" << std::endl;
    });

    h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length)
    {
        int i = 12;
    });

    h.onError([](void *user) {
        int i = 12;
    });

    h.onMessage([&h, &gameMap](uWS::WebSocket<uWS::SERVER> *ws, char *data, size_t length, uWS::OpCode opCode)
    {
        std::string msgString = std::string(data, length);

        if (StringUtils::endsWith(msgString,"\r"))
            msgString = msgString.substr(0, msgString.length()-1);
        if (StringUtils::endsWith(msgString,"\n"))
            msgString = msgString.substr(0, msgString.length()-1);

        std::vector <std::string> parts = StringUtils::split(msgString, '|');

        std::string playerToken = parts.at(1);

        // Validate the playertoken
        if (!MapUtils::mapContains(gameMap, playerToken))
        {
            std::cout << "dspr-gameserver: Received invalid playertoken: " << playerToken << std::endl;
            return;
        }

        // Get the game associated with the token
        Game* game = gameMap.at(playerToken);

        // Process message
        if (parts.at(0).compare("auth/1.0/gametoken") == 0)
        {
            std::cout << "dspr-gameserver: Received 'auth/1.0/gametoken', added player to game" << std::endl;
            game->addPlayer(playerToken, ws);
        }
        else if (parts.at(0).compare("unit/1.0/order") == 0)
        {
            std::cout << "dspr-gameserver: Received '" << msgString << "'" << std::endl;
        }
    });

    auto timer = new Timer(h.getLoop());
    timer->start([](Timer* timer){
        GameServer::get().step();
    }, 200, 200);

    h.getDefaultGroup<uWS::SERVER>().startAutoPing(30000);
    if (h.listen("localhost", 3173)) {
        std::cout << "dspr-gameserver: Listening to port 3173" << std::endl;
    } else {
        std::cerr << "dspr-gameserver: Failed to listen to port" << std::endl;
        return -1;
    }

    h.run();
}
