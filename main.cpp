#include <uWS/uWS.h>
#include <iostream>
#include "Game.h"
#include "StringUtils.h"
#include "MapUtils.h"

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
    });

    h.onMessage([&h](uWS::WebSocket<uWS::SERVER> *ws, char *data, size_t length, uWS::OpCode opCode)
    {
        std::string msgString = std::string(data, length);
        std::vector <std::string> parts = StringUtils::split(msgString, '|');
        if (parts.at(0).compare("auth/1.0/gametoken")){
            std::string token = parts.at(1);
            if (MapUtils::mapContains(gameMap, token)){
                Game* game = gameMap.at(token);
                game->addPlayer(token, ws);
                return;
            }
        }
    });

    h.getDefaultGroup<uWS::SERVER>().startAutoPing(30000);
    if (h.listen("localhost", 3173)) {
        std::cout << "Listening to port 3173" << std::endl;
    } else {
        std::cerr << "Failed to listen to port" << std::endl;
        return -1;
    }

    h.run();
}
