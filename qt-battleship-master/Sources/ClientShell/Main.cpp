#include <iostream>

#include "../NetworkApi/Msg.hpp"
#include "../NetworkApi/MsgGameStatus.hpp"
#include "../NetworkApi/MsgShotAvailable.hpp"
#include "../NetworkApi/MsgPlayerQuery.hpp"
#include "../NetworkApi/MsgPlayerResponse.hpp"
#include "../NetworkApi/MsgShotResults.hpp"
#include "../NetworkApi/ServerPeer.hpp"

/// Прослушиваемый порт
unsigned _port;
/// IP сервера
std::string _ip;
/// Тип подключения к игре
unsigned _connectionType = 0;
/// Ключ игровой сессии
uintptr_t _sessionKey = 0;

// Типы подключения
constexpr unsigned CON_TYPE_NEW = 0;
constexpr unsigned CON_TYPE_JOIN = 1;

/**
 * Точка входа
 * @param argc Кол-во аргументов
 * @param argv Аргументы
 * @return Код выполнения (выхода)
 */
int main(int argc, char* argv[])
{
    // Если ликовка QT статическая, нужно статически импортировать плагин
#ifdef QT_STATIC_BUILD
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

    try
    {
        // Ввод IP адреса
        std::cout << "Please enter IP: ";
        std::getline(std::cin, _ip);

        // Ввод прослушиваемого порта
        std::cout << "Please enter port: ";
        std::cin >> _port;
        std::cin.ignore();

        // Тип подключения (новая сессия или присоединение)
        std::cout << "Please select connection type (0 - new, 1 - join): ";
        std::cin >> _connectionType;
        std::cin.ignore();

        // Ввод ID'а сессии (если нужно)
        if(_connectionType == 1){
            std::cout << "Please enter session ID: ";
            std::cin >> _sessionKey;
        }

        // Объект для взаимодействия с сервером
        net::ServerPeer server(_ip.c_str(),_port);

        // Если не удалось подключиться
        if(!server.isConnected()){
            throw std::runtime_error("Error: Can not establish connection to server.");
        }

        std::cout << "Connected to " << _ip << "(" << _port << ")" << std::endl;

        // Присоединился ли игрок к игре
        bool joined = false;

        // Если запрашиваем новую сессию
        if(_connectionType == CON_TYPE_NEW)
        {
            if(server.sendMessage(net::MsgPlayerQuery())){
                auto response = server.waitForMessage();
                if(response.getType() == net::MSG_PLR_RESPONSE && response.toMsgPlayerResponse().getResponseData().joined){
                    std::cout << "Joined to game. Session key - " << response.toMsgPlayerResponse().getResponseData().sessionKey << std::endl;
                    joined = true;
                }else{
                    //TODO: Handle error
                }
            } else {
                throw std::runtime_error("Error: Can not send query to server.");
            }
        }
        // Если подключаемся к существующей сессии
        else if(_connectionType == CON_TYPE_JOIN)
        {
            if(server.sendMessage(net::MsgPlayerQuery(_sessionKey))){
                auto response = server.waitForMessage();
                if(response.getType() == net::MSG_PLR_RESPONSE && response.toMsgPlayerResponse().getResponseData().joined){
                    std::cout << "Joined to game." << std::endl;
                    joined = true;
                }else{
                    //TODO: Handle error
                }
            }
            else {
                throw std::runtime_error("Error: Can not send query to server.");
            }
        }

        // Если удалось присоединиться к игровой сессии
        if(joined)
        {
            // Ожидаем сообщения о статусе игры
            std::cout << "Waiting for game startup..." << std::endl;
            auto msgGameStartup = server.waitForMessage();

            // Если получили сообщение о статусе игры
            if(msgGameStartup.getType() == net::MSG_GAME_STATUS)
            {
                // Если игра запущена
                if(msgGameStartup.toMsgGameStatus().getStatus() == net::GAME_RUNNING)
                {
                    std::cout << "Game in process." << std::endl;

                    // Запуск основного цикла
                    while(true)
                    {
                        // Ожидаем информацию о том чей ход
                        std::cout << "Whose turn?" << std::endl;
                        auto serverMsg = server.waitForMessage();

                        // Если это информация о ходе
                        if(serverMsg.getType() == net::MSG_SHOT_AVAILABLE)
                        {
                            /// Если игрок ходит
                            if(serverMsg.toMsgShotAvailable().isAvailable())
                            {
                                std::cout << "My turn!" << std::endl;

                                // Ввод хода
                                net::MsgShotDetails::ShotDetails details = {};
                                std::cout << "x: "; std::cin >> details.x;
                                std::cout << "y: "; std::cin >> details.y;
                                std::cin.ignore();

                                // Отправка хода серверу
                                if(server.sendMessage(net::MsgShotDetails(details)))
                                {
                                    std::cout << "Sent. Waiting for answer" << std::endl;
                                    auto msgResult = server.waitForMessage();
                                    if(msgResult.getType() == net::MSG_SHOT_RESULTS){
                                        std::cout << "Answer received: ";
                                        uint8_t result = msgResult.toMsgShotResults().getResults();

                                        if(result == net::SHOT_RESULT_MISS){
                                            std::cout << "Miss" << std::endl;
                                        }else if(result == net::SHOT_RESULT_HIT){
                                            std::cout << "Hit" << std::endl;
                                        }else if(result == net::SHOT_RESULT_DESTROYED){
                                            std::cout << "Destroyed" << std::endl;
                                        }else if(result == net::SHOT_RESULT_WIN){
                                            std::cout << "Win" << std::endl;
                                        }else{
                                            std::cout << "Unrecognized (" << result << ")" << std::endl;
                                        }
                                    }else{
                                        //TODO: Handle error
                                    }
                                }
                                else {
                                    throw std::runtime_error("Error: Can not send to server.");
                                }
                            }
                            /// Если игрок ожидает
                            else
                            {
                                std::cout << "2nd player's turn. Waiting..." << std::endl;

                                // Ожидаем информацию о ходе
                                auto shotDetails = server.waitForMessage();
                                if(shotDetails.getType() == net::MSG_SHOT_DETAILS)
                                {
                                    // Вывод информации о ходе противника
                                    auto details = shotDetails.toMsgShotDetails().getDetails();
                                    std::cout << "2nd players shot: x = " << details.x << ", y = " << details.y << std::endl;

                                    // Ввод ответа (попал, не попал и прочее)
                                    uint8_t result;
                                    short iResult;
                                    std::cout << "Answer to player (0 - miss, 1 - hit, 2 - destroyed, 3 - win): ";
                                    std::cin >> iResult;
                                    result = static_cast<uint8_t>(iResult);
                                    std::cin.ignore();

                                    // Отправка ответа
                                    if(server.sendMessage(net::MsgShotResults(result))){
                                        std::cout << "Answer sent. " << std::endl;
                                    }else{
                                        throw std::runtime_error("Error: Can not send to server.");
                                    }
                                }else{
                                    //TODO: Handle error
                                }
                            }
                        }
                        else if(serverMsg.getType() == net::MSG_GAME_STATUS) {
                            switch(serverMsg.toMsgGameStatus().getStatus())
                            {
                                case net::GAME_OVER_DISCONNECTED:
                                    std::cout << "2nd player disconnected." << std::endl;
                                    break;
                                case net::GAME_OVER_WIN:
                                    std::cout << "You win" << std::endl;
                                    break;
                                case net::GAME_OVER_LOOSE:
                                    std::cout << "You loose" << std::endl;
                                    break;
                            }
                            break;
                        } else {
                            std::cout << "Unexpected message type. Expected types -"
                                      << (int)net::MSG_SHOT_AVAILABLE << ", " << (int)net::MSG_GAME_STATUS << " got - "
                                      << (int)serverMsg.getType()
                                      << std::endl;
                        }
                    }
                } else {
                    std::cout << "Can't start game. Expected status - "
                              << (int)net::GAME_RUNNING << ", got - "
                              << (int)msgGameStartup.toMsgGameStatus().getStatus()
                              << std::endl;
                }
            } else {
                std::cout << "Received wrong message. Expected - "
                          << (int)net::MSG_GAME_STATUS << ", got - "
                          << (int)msgGameStartup.getType()
                          << std::endl;
            }
        }
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        system("pause");
    }

    system("pause");
    return 0;
}