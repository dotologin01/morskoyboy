#pragma once

#include "PlayerPeer.hpp"

#include <vector>
#include <random>

namespace net
{
    class GameSession
    {
    private:
        /// Массив игроков
        std::vector<PlayerPeer> players_;
        /// Индекс активного игрока
        int activePlayerIndex_;

    public:
        /**
         * Конструктор
         */
        GameSession():activePlayerIndex_(0){};

        /**
         * Деструктор
         */
        ~GameSession() = default;

        /**
         * Запрет копирования через инициализацию
         * @param other Ссылка на копируемый объекта
         */
        GameSession(const GameSession& other) = delete;

        /**
         * Запрет копирования через присваивание
         * @param other Ссылка на копируемый объекта
         * @return Ссылка на текущий объект
         */
        GameSession& operator=(const GameSession& other) = delete;

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        GameSession(GameSession&& other) noexcept : activePlayerIndex_(0){
            std::swap(activePlayerIndex_,other.activePlayerIndex_);
            std::swap(players_,other.players_);
        }

        /**
         * Перемещение через присваивание
         * @param other R-value ссылка на другой объект
         * @return Ссылка на текущий объект
         */
        GameSession& operator=(GameSession&& other) noexcept{
            if (this == &other) return *this;

            activePlayerIndex_ = 0;

            std::swap(activePlayerIndex_,other.activePlayerIndex_);
            std::swap(players_,other.players_);

            return *this;
        }

        /**
         * Добавить игрока
         * @param playerPeer Игрок
         * @return Удалось ли добавить ? (можно добавить не более двух)
         */
        bool addPlayer(PlayerPeer&& playerPeer){
            if(players_.size() < 2){
                players_.push_back(std::move(playerPeer));
                return true;
            }
            return false;
        }

        /**
         * Рандомизация индекса активного игрока
         */
        void randomizePlayers(){
            std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<> dis(0, 1);
            activePlayerIndex_ = dis(gen);
        }

        /**
         * Сменить игроков местами
         */
        void swapPlayers(){
            activePlayerIndex_ = activePlayerIndex_ == 0 ? 1 : 0;
        }

        /**
         * Получить активного игрока (тот кто ходит)
         * @return Ссылка на объект игрока
         */
        PlayerPeer& getActivePlayer(){
            return players_[activePlayerIndex_];
        }

        /**
         * Получить ожидающего игрока (тот кто ожидает хода противника)
         * @return Ссылка на объект игрока
         */
        PlayerPeer& getWaitingPlayer(){
            return players_[activePlayerIndex_ == 0 ? 1 : 0];
        }

        /**
         * Все ли игроки подключены
         * @return Да или нет
         */
        bool allConnected(){
            for(PlayerPeer& player : players_){
                if(!player.isConnected()){
                    return false;
                }
            }
            return true;
        }

        /**
         * Отправить сообщение всем подключенным игрокам
         * @param message Сообщение
         */
        void sendToConnected(const Msg& message){
            for(PlayerPeer& player : players_){
                if(player.isConnected()){
                    player.sendMessage(message);
                }
            }
        }
    };
}
