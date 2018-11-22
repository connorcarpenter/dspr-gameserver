#include <uWS/uWS.h>
#include <iostream>
#include "Game/Game.h"
#include "StringUtils.h"
#include "GameServer.h"
#include "Game/Unit/UnitManager.h"
#include "PlayerData.h"
#include "MessageReceiver.h"

using namespace DsprGameServer;

int main()
{
    std::string bffToken = "nwlrbbmqbhcdacon";

    uWS::Hub h;
    MessageReceiver mr;

    h.onConnection([&h](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
    {
        PlayerData* playerData = new PlayerData(ws);
        ws->setUserData(playerData);

        DsprMessage::ToClientMsg authGameTokenMsgV1;
        authGameTokenMsgV1.msgType.set((unsigned char) DsprMessage::ToClientMsg::MessageType::AuthGameToken);
        auto packedMsg = authGameTokenMsgV1.Pack();
        GameServer::get().queueMessage(playerData, packedMsg);
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

    h.onMessage([&h, &bffToken, &mr](uWS::WebSocket<uWS::SERVER> *ws, char *data, size_t length, uWS::OpCode opCode)
    {
        PlayerData* playerData = (PlayerData*) ws->getUserData();

        std::string msgString = std::string(data, length);

        if (StringUtils::endsWith(msgString,"\r"))
            msgString = msgString.substr(0, msgString.length()-1);
        if (StringUtils::endsWith(msgString,"\n"))
            msgString = msgString.substr(0, msgString.length()-1);

        mr.receive(playerData, bffToken, msgString);
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
