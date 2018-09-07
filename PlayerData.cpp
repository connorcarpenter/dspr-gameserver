//
// Created by connor on 9/7/18.
//

#include "PlayerData.h"
#include "Game/Game.h"
#include "GameServer.h"

namespace DsprGameServer
{
    PlayerData::PlayerData(uWS::WebSocket<1> *ws) {
        this->ws = ws;
    }

    PlayerData::~PlayerData()
    {
        if (this->currentGame != nullptr){
            this->currentGame->removePlayer(this);
        }

        GameServer::get().removePlayerToken(this->token);
    }

    uWS::WebSocket<1> * PlayerData::getWs()
    {
        return this->ws;
    }

    void PlayerData::setToken(std::string token) {
        this->token = token;
    }

    void PlayerData::setCurrentGame(Game *game) {
        this->currentGame = game;
    }

    void PlayerData::setTribe(DsprGameServer::Tribe *tribe) {
        this->currentTribe = tribe;
    }
}