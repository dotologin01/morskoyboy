#pragma once

#include <QTcpServer>
#include <QTcpSocket>

namespace net
{
    /// Типы сообщений

    // Тип сообщения - не определен
    constexpr uint8_t MSG_UNDEFINED = 0;
    // Тип сообщения - запрос игрока на подключение к игре
    constexpr uint8_t MSG_PLR_QUERY = 1;
    // Тип сообщения - ответ сервера на запрос игрока
    constexpr uint8_t MSG_PLR_RESPONSE = 2;
    // Тип сообщения - состояние игры
    constexpr uint8_t MSG_GAME_STATUS = 3;
    // Тип сообщения - доступность хода (выстрела)
    constexpr uint8_t MSG_SHOT_AVAILABLE = 4;
    // Тип сообщения - детали хода (выстрела)
    constexpr uint8_t MSG_SHOT_DETAILS = 5;
    // Тип сообщения - итоги хода (выстрела)
    constexpr uint8_t MSG_SHOT_RESULTS = 6;

    /// Состояние игры

    // Состояние игры - игра в процессе
    constexpr uint8_t GAME_RUNNING = 0;
    // Состояние игры - игра закончилась победой
    constexpr uint8_t GAME_OVER_WIN = 1;
    // Состояние игры - игра закончилась поражением
    constexpr uint8_t GAME_OVER_LOOSE = 2;
    // Состояние игры - игра закончилась отключением второго игрока
    constexpr uint8_t GAME_OVER_DISCONNECTED = 3;

    /// Итоги хода

    // Итог хода - промах
    constexpr uint8_t SHOT_RESULT_MISS = 0;
    // Итог хода - попадание
    constexpr uint8_t SHOT_RESULT_HIT = 1;
    // Итог хода - уничтожение
    constexpr uint8_t SHOT_RESULT_DESTROYED = 2;
    // Итог хода - победа (уничтожен последний корабль)
    constexpr uint8_t SHOT_RESULT_WIN = 3;

    /// Предварительное объявление
    class MsgGameStatus;
    class MsgShotAvailable;
    class MsgShotDetails;
    class MsgShotResults;
    class MsgPlayerQuery;
    class MsgPlayerResponse;

    /**
     * Базовый класс игрового сообщения
     * В процессе игры игрок обменивается с сервером унифицированными пакетами (сообщениями)
     */
    class Msg
    {
    private:
        /// Классы PlayerPeer и ServerPeer имеет доступ к закрытым членам данного класса
        friend class BasePeer;
        friend class PlayerPeer;
        friend class ServerPeer;

    protected:
        /// Тип сообщения
        uint8_t type_;
        /// Размер полезной нагрузки
        size_t payloadSize_;
        /// Полезная нагрузка
        char* payload_;

    public:
        /**
         * Конструктор
         * @param type Тип сообщения
         * @param payloadSize Размер полезной нагрузке
         */
        explicit Msg(uint8_t type, size_t payloadSize):
                type_(type),
                payloadSize_(payloadSize),
                payload_(payloadSize > 0 ? new char[payloadSize] : nullptr)
        {}

        /**
         * Деструктор
         */
        virtual ~Msg(){
            delete[] payload_;
        }

        /**
         * Конструктор копирования
         * @param other Ссылка на копируемый объекта
         */
        Msg(const Msg& other):
                type_(other.type_),
                payloadSize_(other.payloadSize_),
                payload_(other.payloadSize_ > 0 ? new char[other.payloadSize_] : nullptr)
        {
            if(other.payloadSize_ > 0){
                memcpy(payload_,other.payload_,payloadSize_);
            }
        }

        /**
         * Оператор присвоения через копирование
         * @param other Ссылка на копируемый объекта
         * @return Ссылка на текущий объект
         */
        Msg& operator=(const Msg& other)
        {
            if (this == &other) return *this;
            delete[] payload_;
            payload_ = nullptr;

            type_ = other.type_;
            payloadSize_ = other.payloadSize_;
            if(other.payloadSize_ > 0){
                payload_ = new char[other.payloadSize_];
                memcpy(payload_,other.payload_,payloadSize_);
            }

            return *this;
        }

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         */
        Msg(Msg&& other) noexcept :
                type_(MSG_UNDEFINED),
                payloadSize_(0),
                payload_(nullptr)
        {
            std::swap(type_,other.type_);
            std::swap(payloadSize_,other.payloadSize_);
            std::swap(payload_,other.payload_);
        }

        /**
         * Перемещение через присваивание
         * @param other R-value ссылка на другой объект
         * @return Ссылка на текущий объект
         */
        Msg& operator=(Msg&& other) noexcept
        {
            if (this == &other) return *this;
            delete[] payload_;
            payload_ = nullptr;

            std::swap(type_,other.type_);
            std::swap(payloadSize_,other.payloadSize_);
            std::swap(payload_,other.payload_);

            return *this;
        }

        /**
         * Получить тип сообщения
         * @return Тип
         */
        uint8_t getType() const{
            return type_;
        }

        /**
         * Конвертировать в MsgGameStatus
         * @return Ссылка на текущий объект
         */
        MsgGameStatus& toMsgGameStatus(){
            return *(reinterpret_cast<MsgGameStatus*>(this));
        }

        /**
         * Конвертировать в MsgShotAvailable
         * @return Ссылка на текущий объект
         */
        MsgShotAvailable& toMsgShotAvailable(){
            return *(reinterpret_cast<MsgShotAvailable*>(this));
        }

        /**
         * Конвертировать в MsgShotDetails
         * @return Ссылка на текущий объект
         */
        MsgShotDetails& toMsgShotDetails(){
            return *(reinterpret_cast<MsgShotDetails*>(this));
        }

        /**
         * Конвертировать в MsgShotResults
         * @return Ссылка на текущий объект
         */
        MsgShotResults& toMsgShotResults(){
            return *(reinterpret_cast<MsgShotResults*>(this));
        }

        /**
         * Конвертировать в MsgPlayerQuery
         * @return Ссылка на текущий объект
         */
        MsgPlayerQuery& toMsgPlayerQuery(){
            return *(reinterpret_cast<MsgPlayerQuery*>(this));
        }

        /**
         * Конвертировать в MsgPlayerResponse
         * @return Ссылка на текущий объект
         */
        MsgPlayerResponse& toMsgPlayerResponse(){
            return *(reinterpret_cast<MsgPlayerResponse*>(this));
        }
    };
}
