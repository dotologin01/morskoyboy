#include <iostream>
#include <unordered_map>
#include <QtPlugin>

#include "../NetworkApi/Msg.hpp"
#include "../NetworkApi/MsgGameStatus.hpp"
#include "../NetworkApi/MsgShotAvailable.hpp"
#include "../NetworkApi/MsgPlayerQuery.hpp"
#include "../NetworkApi/MsgPlayerResponse.hpp"
#include "../NetworkApi/MsgShotResults.hpp"
#include "../NetworkApi/PlayerPeer.hpp"
#include "../NetworkApi/GameSession.hpp"

/// Прослушиваемый порт
unsigned _port;
/// Состояние сервера
bool _serverOn = true;
/// Ассоциативный игровых массив сессий
std::unordered_map<uintptr_t,net::GameSession> _sessions;

/**
 * Процедура игровой сессии (работает в отдельном потоке)
 * @param sessionKey Ключ сессии
 */
void sessionProcedure(uintptr_t sessionKey);

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
        // Ввод прослушиваемого порта
        std::cout << "Please enter port: ";
        std::cin >> _port;

        // Инициализация TCP сервера
        QTcpServer tcpServer;
        if(!tcpServer.listen(QHostAddress::Any, static_cast<quint16>(_port))){
            throw std::runtime_error("Error: can't open listening socket.");
        }

        std::cout << "Listening port (" << _port << ")." << std::endl;

        // Основной цикл сервера
        while(_serverOn)
        {
            // Ожидать новых подключений
            tcpServer.waitForNewConnection(-1);

            // Получить сокет подключившегося клиента
            QTcpSocket* clientSocket = tcpServer.nextPendingConnection();

            // Если соединение установлено
            if(clientSocket->state() == QAbstractSocket::ConnectedState)
            {
                // Информация о клиенте
                std::cout << "Client " << clientSocket << " connected (" << clientSocket->peerAddress().toString().toStdString() << ")" << std::endl;

                // Создать объект для взаимодействия с игроком
                net::PlayerPeer player(clientSocket);

                // Далее игрок игрок, сразу же после подключения, отправляет запрос (сообщение) на присоединение к игре
                // Ожидаем его с небольшим тайм-аутом
                net::Msg playerQuery = player.waitForMessage(100);

                // Если это сообщение о подключении к игре
                if(playerQuery.getType() == net::MSG_PLR_QUERY){

                    // Если игрок НЕ подключается к сессии, но создает НОВУЮ
                    if(playerQuery.toMsgPlayerQuery().newSession())
                    {
                        // Получить уникальный ключ сессии
                        auto sessionKey = reinterpret_cast<uintptr_t>(clientSocket);

                        std::cout << "Client " << clientSocket << " queries new session (" << sessionKey << ") " << std::endl;

                        // Если удалось отправить игроку ответ
                        if(player.sendMessage(net::MsgPlayerResponse(true,sessionKey)))
                        {
                            // Добавить в сессию игрока
                            _sessions[sessionKey].addPlayer(std::move(player));
                            std::cout << "New session created. Key sent to client." << std::endl;
                        }
                        // Если не удалось
                        else{
                            std::cout << "Session not created. Can't send response to client." << std::endl;
                        }
                    }
                    // Если игрок подключается к СУЩЕСТВУЮЩЕЙ сессии
                    else
                    {
                        // Получить уникальный ключ сессии
                        auto sessionKey = playerQuery.toMsgPlayerQuery().getSessionKey();

                        std::cout << "Client " << clientSocket << "joins to existing session (" << sessionKey << ")" << std::endl;

                        // Если удалось найти сессию по ключу и второй игрок не отключился
                        if(_sessions.find(sessionKey) != _sessions.end() && _sessions[sessionKey].allConnected()){
                            // Если удалось отправить игроку ответ
                            if(player.sendMessage(net::MsgPlayerResponse(true)))
                            {
                                // Добавить в сессию игрока
                                _sessions[sessionKey].addPlayer(std::move(player));
                                std::cout << "Player added to session. Response sent to client" << std::endl;

                                // Запустить поток сессии
                                std::thread sessionThread(sessionProcedure,sessionKey);
                                sessionThread.detach();
                            }
                            // Если не удалось
                            else{
                                std::cout << "Player not added. Can't send response to client." << std::endl;
                            }
                        }
                        // Если не удалось найти сессию
                        else{
                            std::cout << "Session with key " << sessionKey << " not found." << std::endl;
                            player.sendMessage(net::MsgPlayerResponse(false));
                        }
                    }
                }
                // Если вместо сообщения о подключении пришло что-то иное
                else {
                    std::cout << "Wrong initial query provided from client " << clientSocket << "(" << clientSocket->peerAddress().toString().toStdString() << "). Ignored." << std::endl;
                }
            }
        }
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        system("pause");
    }
    return 0;
}

/**
 * Процедура игровой сессии (работает в отдельном потоке)
 * @param sessionKey Ключ сессии
 */
void sessionProcedure(uintptr_t sessionKey)
{
    // Если вдруг такой сессии нет
    if(_sessions.find(sessionKey) == _sessions.end())
        return;

    // Ссылка на текущую сессию
    net::GameSession& s = _sessions[sessionKey];

    // Рандомизация игроков
    s.randomizePlayers();

    // Отправить сообщение о статусе игры
    if(s.allConnected()){
        s.sendToConnected(net::MsgGameStatus(net::GAME_RUNNING));
    }else{
        s.sendToConnected(net::MsgGameStatus(net::GAME_OVER_DISCONNECTED));
    }

    // Основной цикл процедуры
    while(true)
    {
        // Если кто-то отключен - отправляем сообщение об остановке игры
        if(!s.allConnected()){
            s.sendToConnected(net::MsgGameStatus(net::GAME_OVER_DISCONNECTED));
            break;
        }
        // Иначе отправить игрокам сообщение о том кто ходит а кто нет
        else{
            s.getActivePlayer().sendMessage(net::MsgShotAvailable(true));
            s.getWaitingPlayer().sendMessage(net::MsgShotAvailable(false));
        }

        // Ожидаем хода активного игрока, получаем сообщение с информацией о ходе
        auto shotMsg = s.getActivePlayer().waitForMessage();

        // Если получено сообщение MSG_UNDEFINED и какие-то игроки отключены
        if(shotMsg.getType() == net::MSG_UNDEFINED && !s.allConnected()){
            s.sendToConnected(net::MsgGameStatus(net::GAME_OVER_DISCONNECTED));
            break;
        }
        // Иначе отправляем информацию о ходе ожидающему игроку
        else{
            s.getWaitingPlayer().sendMessage(shotMsg);
        }

        // Ответ от ожидающего игрока (промазал, попал, уничтожил, победил)
        auto shotResults = s.getWaitingPlayer().waitForMessage();

        // Если получено сообщение MSG_UNDEFINED и какие-то игроки отключены
        if(shotResults.getType() == net::MSG_UNDEFINED && !s.allConnected()){
            s.sendToConnected(net::MsgGameStatus(net::GAME_OVER_DISCONNECTED));
            break;
        }
        // Иначе отправляем ответ ходившему игроку
        else{
            s.getActivePlayer().sendMessage(shotResults);
        }

        // Если ходивший игрок победил (уничтожил последний корабль) - отправить игрокам сообщения о завершении игры
        if(shotResults.toMsgShotResults().getResults() == net::SHOT_RESULT_WIN){
            s.getActivePlayer().sendMessage(net::MsgGameStatus(net::GAME_OVER_WIN));
            s.getWaitingPlayer().sendMessage(net::MsgGameStatus(net::GAME_OVER_LOOSE));
            break;
        }
        // Если ходивший промазал - сменить игроков (итерация начинается заново)
        else if (shotResults.toMsgShotResults().getResults() == net::SHOT_RESULT_MISS){
            s.swapPlayers();
        }
    }

    // Завершение сессии
    _sessions.erase(sessionKey);
    std::cout << "Session (" << sessionKey << ") closed." << std::endl;
}