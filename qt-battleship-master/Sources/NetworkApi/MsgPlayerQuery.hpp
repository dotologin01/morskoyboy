#pragma once

#include "Msg.hpp"

namespace net
{
    /**
     * Сообщение о подключении игрока к игре
     */
    class MsgPlayerQuery final : public Msg
    {
    public:
        explicit MsgPlayerQuery(uintptr_t sessionKey = 0): Msg(MSG_PLR_QUERY, sizeof(uintptr_t)){
            memcpy(this->payload_, &sessionKey, sizeof(uintptr_t));
        }

        uintptr_t getSessionKey(){
            return *(reinterpret_cast<uintptr_t*>(payload_));
        }

        bool newSession(){
            return this->getSessionKey() == 0;
        }
    };
}


