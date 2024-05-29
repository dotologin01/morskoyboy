#include "GameWindow.h"
#include "GameStartWindow.h"
#include "./ui_GameStartWindow.h"

#include "../NetworkApi/MsgPlayerQuery.hpp"
#include "../NetworkApi/ServerPeer.hpp"

/// Настройки - IP сервера
extern QString _ip;
/// Настройки - Порт сервера
extern unsigned _port;
/// Объект для взаимодействия с сервером
extern net::ServerPeer* _server;

/**
 * Конструктор окна
 * @param parent Родительский элемент
 */
GameStartWindow::GameStartWindow(GameWindow *mainWindow):
        QWidget(nullptr),
        gameWindow_(mainWindow),
        ui_(new Ui::GameStartWindow)
{
    // Инициализация UI
    ui_->setupUi(this);
}

/**
 * Деструктор окна
 */
GameStartWindow::~GameStartWindow()
{
    delete ui_;
}

/// S L O T S

/**
 * Обработка события нажатия на кнопку создания новой сессии
 */
void GameStartWindow::on_btnNewSession_clicked()
{
    // Попытка создать сервер-peer
    _server = new net::ServerPeer(_ip.toStdString().c_str(), _port);

    // Если удалось подключиться
    if(_server->isConnected()){
        // Отправляем серверу сообщение о запросе новой игровой сессии
        _server->sendMessage(net::MsgPlayerQuery());
        // Тут же ожидаем ответа от сервера
        auto response = _server->waitForMessage();
        // Если пришел ответ и игрок был присоединен к новой сессии
        if(response.getType() == net::MSG_PLR_RESPONSE && response.toMsgPlayerResponse().getResponseData().joined)
        {
            // Вывести ключ сессии
            this->ui_->editSessionKeyNew->setText(QString::number(response.toMsgPlayerResponse().getResponseData().sessionKey));
            // Сменить состояние
            this->gameWindow_->currentState_ = GameWindow::GameClientState::CONNECTED_NEW;
            this->gameWindow_->onStateChange();
            // Сделать таб подключения к существующей сессии не активным
            this->ui_->tabJoinToSession->setEnabled(false);
            // Сделать кнопку создания сессии не активной
            this->ui_->btnNewSession->setEnabled(false);

            // Обработчик события готовности сокета к чтению
            connect(_server->getSocket(),SIGNAL(readyRead()),this->gameWindow_,SLOT(onReadyReadServerMessage()));
        }
        // Если пришел не корректный ответ или игрок не был присоединен
        else {
            // Сообщение
            QMessageBox msgBox;
            msgBox.setWindowTitle("Ошибка.");
            msgBox.setText("Ошибка на сервере. Сервер отправил не корректный ответ.");
            msgBox.setIcon(QMessageBox::Icon::Critical);
            msgBox.exec();
        }
    }
    // Если подключение не удалось
    else{
        // Сообщение
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка.");
        msgBox.setText("Невозможно наладить подключение с игровым сервером. Убедитесь что настройки подключения корректны и сервер доступен.");
        msgBox.setIcon(QMessageBox::Icon::Critical);
        msgBox.exec();
    }
}

/**
 * Обработка события нажатия на кнопку подключения к сессии
 */
void GameStartWindow::on_btnJoin_clicked()
{
    // Попытка создать сервер-peer
    _server = new net::ServerPeer(_ip.toStdString().c_str(), _port);

    // Если удалось подключиться
    if(_server->isConnected()){
        // Отправляем серверу сообщение о запросе новой игровой сессии
        _server->sendMessage(net::MsgPlayerQuery(this->ui_->editSessionKeyJoin->text().toUInt()));
        // Тут же ожидаем ответа от сервера
        auto response = _server->waitForMessage();
        // Если пришел ответ и игрок был присоединен к новой сессии
        if(response.getType() == net::MSG_PLR_RESPONSE && response.toMsgPlayerResponse().getResponseData().joined)
        {
            // Сменить состояние
            this->gameWindow_->currentState_ = GameWindow::GameClientState::CONNECTED_JOINED;
            this->gameWindow_->onStateChange();
            // Сделать таб создания новой сессии не активным
            this->ui_->tabNewSession->setEnabled(false);
            // Сделать кнопку подключения к сессии не активной
            this->ui_->btnJoin->setEnabled(false);
            // Закрыть текущее окно
            this->close();

            // Обработчик события готовности сокета к чтению
            connect(_server->getSocket(),SIGNAL(readyRead()),this->gameWindow_,SLOT(onReadyReadServerMessage()));
        }
        // Если пришел не корректный ответ или игрок не был присоединен
        else {
            // Сообщение
            QMessageBox msgBox;
            msgBox.setWindowTitle("Ошибка.");
            msgBox.setText("Вероятно такая сессия не существует, либо произошла ошибка на сервере. Попробуйте еще раз");
            msgBox.setIcon(QMessageBox::Icon::Critical);
            msgBox.exec();
        }
    }
    // Если подключение не удалось
    else{
        // Сообщение
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка.");
        msgBox.setText("Невозможно наладить подключение с игровым сервером. Убедитесь что настройки подключения корректны и сервер доступен.");
        msgBox.setIcon(QMessageBox::Icon::Critical);
        msgBox.exec();
    }
}
