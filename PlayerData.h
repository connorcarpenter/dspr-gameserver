//
// Created by connor on 9/7/18.
//

#include <uWS/WebSocket.h>

namespace DsprGameServer
{

    class Game;

    class PlayerData
    {

    public:

        explicit PlayerData(uWS::WebSocket<1> *ws);
        ~PlayerData();

        uWS::WebSocket<1> * getWs();

        void setToken(std::string token);

        void setCurrentGame(Game* game);
        Game* getCurrentGame();

    private:

        uWS::WebSocket<1> *ws = nullptr;
        std::string token;
        Game* currentGame = nullptr;
    };
}