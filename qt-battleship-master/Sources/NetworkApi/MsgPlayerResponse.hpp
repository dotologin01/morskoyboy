#pragma once

#include "Msg.hpp"

namespace net
{
    /**
     * Сообщение о деталях хода (координаты выстрела)
     */
    class MsgPlayerResponse final : public Msg
    {
    public:
        struct PlayerResponse{
            bool joined;
            uintptr_t sessionKey;
        };

        explicit MsgPlayerResponse(const PlayerResponse& details):Msg(MSG_PLR_RESPONSE, sizeof(PlayerResponse)){
            memcpy(this->payload_,&details, sizeof(PlayerResponse));
        }

        explicit MsgPlayerResponse(bool joined, uintptr_t sessionKey = 0):Msg(MSG_PLR_RESPONSE, sizeof(PlayerResponse)){
            PlayerResponse response = {};
            response.joined = joined;
            response.sessionKey = sessionKey;
            memcpy(this->payload_,&response, sizeof(PlayerResponse));
        }

        PlayerResponse getResponseData(){
            return *(reinterpret_cast<PlayerResponse*>(payload_));
        }
    };
}