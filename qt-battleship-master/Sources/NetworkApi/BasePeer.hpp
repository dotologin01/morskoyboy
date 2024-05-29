#pragma once

#include "Msg.hpp"
#include "MsgShotDetails.hpp"
#include "MsgPlayerResponse.hpp"

#include <thread>
#include <chrono>

namespace net
{
    /**
     * Базовый класс для работы с соединением игрок-сервер и сервер-игрок
     */
    class BasePeer
    {
    protected:
        /// Подключение (сокет)
        QTcpSocket* connection_;

    public:
        /**
         * Конструктор
         * @param connectionSocket Соединение
         */
        explicit BasePeer(QTcpSocket* connectionSocket):
                connection_(connectionSocket){}

        /**
        * Очистка
        */
        ~BasePeer(){
            delete connection_;
        }

        /**
         * Запрет копирования через инициализацию
         * @param other Ссылка на копируемый объекта
         */
        BasePeer(const BasePeer& other) = delete;

        /**
         * Запрет копирования через присваивание
         * @param other Ссылка на копируемый объекта
         * @return Ссылка на текущий объект
         */
        BasePeer& operator=(const BasePeer& other) = delete;

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        BasePeer(BasePeer&& other) noexcept : connection_(nullptr){
            std::swap(connection_,other.connection_);
        }

        /**
         * Перемещение через присваивание
         * @param other R-value ссылка на другой объект
         * @return Ссылка на текущий объект
         */
        BasePeer& operator=(BasePeer&& other) noexcept{
            if (this == &other) return *this;

            delete connection_;
            connection_= nullptr;

            std::swap(connection_,other.connection_);

            return *this;
        }

        /**
         * Получить сокет
         * @return Указатель на сокет
         */
        QTcpSocket* getSocket(){
            return connection_;
        }

        /**
         * Читать сообщение
         * @param maxAttempts Количество попыток при неудачном прочтении полезной нагрузки
         * @return Объект сообщения
         */
        Msg readMessage(unsigned maxAttempts = 10){
            // Тип сообщения
            uint8_t msgType = MSG_UNDEFINED;
            // Размер полезной нагрузки
            size_t payloadSize = 0;

            // Ожидать готовности чтения
            if(connection_ != nullptr && connection_->state() == QTcpSocket::ConnectedState)
            {
                // Прочесть первый байт (это тип сообщения)
                connection_->read(reinterpret_cast<char*>(&msgType), sizeof(uint8_t));

                // В зависимости от типа определить размер полезной нагрузки
                switch(msgType){
                    case MSG_SHOT_AVAILABLE:
                        payloadSize = sizeof(bool);
                        break;
                    case MSG_GAME_STATUS:
                    case MSG_SHOT_RESULTS:
                        payloadSize = sizeof(uint8_t);
                        break;
                    case MSG_SHOT_DETAILS:
                        payloadSize = sizeof(MsgShotDetails::ShotDetails);
                        break;
                    case MSG_PLR_QUERY:
                        payloadSize = sizeof(uintptr_t);
                        break;
                    case MSG_PLR_RESPONSE:
                        payloadSize = sizeof(MsgPlayerResponse::PlayerResponse);
                        break;
                    default:
                        payloadSize = 0;
                        break;
                }
            }

            // Создать объект сообщения
            Msg msg(msgType,payloadSize);

            // Если полезная нагрузка должна быть
            if(payloadSize > 0){
                // Читать покуда не будут прочтены все байты полезной нагрузки и покуда не превышено кол-во попыток (если считать не получается)
                size_t readBytesTotal = 0;
                do {
                    size_t readBytes = connection_->read(msg.payload_,msg.payloadSize_ - readBytesTotal);
                    readBytesTotal += readBytes;
                    if(readBytes == 0) maxAttempts--;
                } while(readBytesTotal < msg.payloadSize_ && maxAttempts > 0);
            }

            // Вернуть сообщение
            return msg;
        }

        /**
         * Ожидать сообщения
         * @param timeout Время ожидания получения
         * @param maxAttempts Количество попыток при неудачном прочтении полезной нагрузки
         * @return Объект сообщения
         */
        Msg waitForMessage(int timeout = -1, unsigned maxAttempts = 10){
            // Ожидать готовности чтения
            if(connection_ != nullptr && connection_->waitForReadyRead(timeout))
            {
                return this->readMessage(maxAttempts);
            }
            return Msg(MSG_UNDEFINED, 0);
        }

        /**
         * Отправка сообщения
         * @param message Сообщение
         * @param timeout Время ожидания окончания записи данных
         * @param delayBeforeSend Пауза перед началом отправки
         * @return Удалось ли отправить
         */
        bool sendMessage(const Msg& message, int timeout = -1, int delayBeforeSend = 10){
            // Сделать паузу перед отправкой (дабы избежать слияния с предыдущей отправкой)
            if(delayBeforeSend > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(delayBeforeSend));

            // Отправить данные в сокет
            if(connection_ != nullptr){
                connection_->write(reinterpret_cast<const char*>(&message.type_), sizeof(uint8_t));
                connection_->write(message.payload_,message.payloadSize_);
                return connection_->waitForBytesWritten(timeout);
            }

            return false;
        }

        /**
         * Подключен ли игрок
         * @return Да или нет
         */
        bool isConnected(){
            return (this->connection_ != nullptr) && (this->connection_->state() == QTcpSocket::ConnectedState);
        }
    };
}
