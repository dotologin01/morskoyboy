#pragma once

#include "BasePeer.hpp"

namespace net
{
    /**
     * Класс для взаимодействия с сервером
     */
    class ServerPeer final : public BasePeer
    {
    public:
        /**
         * Конструктор
         * @param ip IP
         * @param port Порт
         */
        explicit ServerPeer(const char* ip, unsigned port):BasePeer(new QTcpSocket){
            connection_->connectToHost(QString::fromStdString(ip),port);
            connection_->waitForConnected(-1);
        }
    };
}