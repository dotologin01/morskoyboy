#pragma once

#include "BasePeer.hpp"

namespace net
{
    /**
     * Класс для взаимодействия с игроком
     */
    class PlayerPeer final : public BasePeer{
        // Наследовать все конструкторы
        using BasePeer::BasePeer;
    };
}

