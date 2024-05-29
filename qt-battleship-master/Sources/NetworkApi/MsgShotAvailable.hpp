#pragma once

#include "Msg.hpp"

namespace net
{
    /**
     * Сообщение о доступности хода
     */
    class MsgShotAvailable final : public Msg
    {
    public:
        explicit MsgShotAvailable(bool available):Msg(MSG_SHOT_AVAILABLE, sizeof(bool)){
            memcpy(this->payload_,&available, sizeof(bool));
        };

        bool isAvailable(){
            return *(reinterpret_cast<bool *>(payload_));
        }
    };
}