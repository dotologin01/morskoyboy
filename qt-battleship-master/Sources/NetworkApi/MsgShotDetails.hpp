#pragma once

#include "Msg.hpp"

namespace net
{
    /**
     * Сообщение о деталях хода (координаты выстрела)
     */
    class MsgShotDetails final : public Msg
    {
    public:
        struct ShotDetails{
            size_t x;
            size_t y;
        };

        explicit MsgShotDetails(const ShotDetails& details):Msg(MSG_SHOT_DETAILS, sizeof(ShotDetails)){
            memcpy(this->payload_,&details, sizeof(ShotDetails));
        }

        ShotDetails getDetails(){
            return *(reinterpret_cast<ShotDetails*>(payload_));
        }
    };
}