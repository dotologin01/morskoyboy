#pragma once

#include "Msg.hpp"

namespace net
{
    /**
     * Сообщение о результатах хода
     */
    class MsgShotResults final : public Msg
    {
    public:
        explicit MsgShotResults(uint8_t results):Msg(MSG_SHOT_RESULTS, sizeof(uint8_t)){
            memcpy(this->payload_,&results, sizeof(uint8_t));
        }

        uint8_t getResults(){
            return *(reinterpret_cast<uint8_t*>(payload_));
        }
    };
}