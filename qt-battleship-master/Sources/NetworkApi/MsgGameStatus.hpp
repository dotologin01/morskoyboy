#pragma once

#include "Msg.hpp"

namespace net
{
    /**
     * Сообщение о состоянии игры
     */
    class MsgGameStatus final : public Msg
    {
    public:
        explicit MsgGameStatus(uint8_t status):Msg(MSG_GAME_STATUS, sizeof(uint8_t)){
            memcpy(this->payload_, &status, sizeof(uint8_t));
        }

        uint8_t getStatus(){
            return *(reinterpret_cast<uint8_t*>(payload_));
        }
    };
}


