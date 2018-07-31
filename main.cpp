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
    gameMap.insert(std::pair<std::string, Game*>("game1", new Game()));

    //network stuff

    uWS::Hub h;

    h.onConnection([&h](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
    {
        ws->send("auth/1.0/gametoken|\r\n", uWS::BINARY);//the \r\n makes it happen on EMSC! Don't remove!
        std::cout << "dspr-gameserver: Received 'onConnection', Sent 'auth/1.0/gametoken|'" << std::endl;
    });

    h.onMessage([&h, &gameMap](uWS::WebSocket<uWS::SERVER> *ws, char *data, size_t length, uWS::OpCode opCode)
    {
        std::string msgString = std::string(data, length);
        if (StringUtils::endsWith(msgString,"\r"))
            msgString = msgString.substr(0, msgString.length()-1);
        std::vector <std::string> parts = StringUtils::split(msgString, '|');
        if (parts.at(0).compare("auth/1.0/gametoken") == 0){
            std::string token = parts.at(1);
            if (MapUtils::mapContains(gameMap, token)){

                std::cout << "dspr-gameserver: Received 'auth/1.0/gametoken', added player to game" << std::endl;

                Game* game = gameMap.at(token);
                game->addPlayer(token, ws);

                return;
            }
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
