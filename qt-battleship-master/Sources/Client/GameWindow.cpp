#include "GameWindow.h"

#include "../NetworkApi/Msg.hpp"
#include "../NetworkApi/MsgGameStatus.hpp"
#include "../NetworkApi/MsgShotDetails.hpp"
#include "../NetworkApi/MsgShotAvailable.hpp"
#include "../NetworkApi/MsgShotResults.hpp"
#include "../NetworkApi/ServerPeer.hpp"

/// Объект для взаимодействия с сервером
extern net::ServerPeer* _server;

/**
 * Инициализация игрового окна
 */
GameWindow::GameWindow(QGraphicsScene* scene) : QGraphicsView(scene)
{
    // Основные настройки отображения окна
    this->setRenderHint(QPainter::Antialiasing);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setBackgroundBrush(QColor(230, 200, 167));
    this->setWindowTitle("Battleship");

    // Создать окно настроек подключения
    this->settingsWindow_ = new SettingsWindow();

    // Создать окно начала игры
    this->gameStartWindow_ = new GameStartWindow(this);

    // Создать игровое поле для игрока
    myField_ = new GameField(30.0,{10,10},GameField::FieldState::PREPARING,this);
    myField_->setPos(0,30);
    myField_->addStartupShips();

    // Создать игровое поле для противника
    enemyField_ = new GameField(30.0,{10,10},GameField::FieldState::ENEMY_PREPARING,this);
    enemyField_->setPos(360,30);
    enemyField_->setShotAtEnemyCallback(GameWindow::shotAtEnemy);

    // Создать label'ы
    QPalette labelPal(palette());
    labelPal.setColor(QPalette::Background, QColor(0,0,0,0));
    for(size_t i = 0; i < 2; i++)
    {
        auto label = new QLabel();
        label->setText(i == 0 ? "Ваше поле" : "Поле противника");
        label->setFont(QFont("Arial",18));
        label->setAutoFillBackground(true);
        label->move(i == 0 ? 30 : 390,7);
        label->setPalette(labelPal);
        labels_.push_back(label);
    }

    // Создать кнопку готовности к игре
    btnReady_ = new QPushButton;
    btnReady_->setText("Готов к игре!");
    btnReady_->setFont(QFont("Arial",15));
    btnReady_->move(390,60);

    // Создать кнопку настроек подключения
    btnSettings_ = new QPushButton;
    btnSettings_->setText("Настройки подключения");
    btnSettings_->setFont(QFont("Arial",15));
    btnSettings_->move(390,120);

    // Связать кнопки с обработчиками событий
    connect(btnReady_,&QPushButton::clicked,this,&GameWindow::onReadyButtonClicked);
    connect(btnSettings_,&QPushButton::clicked,this,&GameWindow::onSettingsButtonClicked);

    // Добавить игровые поля к отрисовке
    this->scene()->addItem(myField_);
    this->scene()->addItem(enemyField_);

    // Добавить label'ы к отрисовке
    for(auto label : labels_){
        this->scene()->addWidget(label);
    }


    // Добавить кнопку к отрисовке
    this->scene()->addWidget(btnReady_);
    this->scene()->addWidget(btnSettings_);
}

/**
 * Де-инициализация игрового окна
 */
GameWindow::~GameWindow()
{
    // Уничтожение дополнительных окон
    delete settingsWindow_;

    // Удаление объектов со сцены
    this->scene()->removeItem(myField_);
    this->scene()->removeItem(enemyField_);

    // Уничтожение объектов
    qDeleteAll(labels_);
    delete btnReady_;
    delete btnSettings_;
    delete myField_;
    delete enemyField_;
}

/**
 * Переопределение события изменения размера
 * @details Достаточно просто оставить обработчик пустым
 */
void GameWindow::resizeEvent(QResizeEvent *) {}

/**
 * Выстрел по вражескому полю
 * @param pos Положение клетки по которой осуществляется выстрел
 * @param gameField Указатель на поле
 * @param gameWindow Указатель на игровое окно (родительское для поля)
 */
void GameWindow::shotAtEnemy(const QPoint &pos, GameField *gameField, GameWindow* gameWindow)
{
    // Если подключение установлено
    if(_server != nullptr && _server->isConnected())
    {
        // Если ячейка не занята (частью корабля или отметкой выстрела)
        if(gameField->isCellEmptyAt(pos))
        {
            // Детали хода (координаты) для сообщения
            auto details = net::MsgShotDetails::ShotDetails{
                    static_cast<size_t>(pos.x()),
                    static_cast<size_t>(pos.y())
            };

            // Отправка сообщения серверу. После отправки отключить поле
            if(_server->sendMessage(net::MsgShotDetails(details))){
                gameWindow->currentState_ = GameClientState::WHOSE_TURN;
            }
            // Если не удалось отправить - игра окончена (связь прервана)
            else{
                gameWindow->currentState_ = GameClientState::ENDGAME_DISCONNECTED;
            }

            gameWindow->onStateChange();
        }
    }
    // Если соединения нет - сменить состояние
    else{
        gameWindow->currentState_ = GameClientState::ENDGAME_DISCONNECTED;
        gameWindow->onStateChange();
    }
}

/**
 * Показать или скрыть кнопки
 * @param enable Показать
 */
void GameWindow::enableButtons(bool enable)
{
    this->btnReady_->setEnabled(enable);
    this->btnReady_->setVisible(enable);
    this->btnSettings_->setEnabled(enable);
    this->btnSettings_->setVisible(enable);
}

/**
 * Вызывается во время смены состояния игрового клиента
 */
void GameWindow::onStateChange()
{
    switch(this->currentState_)
    {
        // Подготовка
        case GameClientState::PREPARING:
            this->enableButtons(true);

            // Состояние полей
            this->myField_->setState(GameField::FieldState::PREPARING);
            this->enemyField_->setState(GameField::FieldState::ENEMY_PREPARING);
            break;

        // Игрок подключен, но игра не началась
        case GameClientState::CONNECTED_NEW:
        case GameClientState::CONNECTED_JOINED:
            // Активировать кнопки настроек
            this->btnReady_->setEnabled(true);
            this->btnReady_->setVisible(true);
            this->btnSettings_->setEnabled(false);
            this->btnSettings_->setVisible(false);

            // Состояние полей
            this->myField_->setState(GameField::FieldState::READY);
            this->enemyField_->setState(GameField::FieldState::ENEMY_PREPARING);
            break;

        // Игра началась, ожидание хода, ход противника
        case GameClientState::WHOSE_TURN:
        case GameClientState::ENEMY_TURN:
            // Спрятать кнопки
            this->enableButtons(false);

            // Состояние полей
            this->myField_->setState(GameField::FieldState::READY);
            this->enemyField_->setState(GameField::FieldState::ENEMY_PREPARING);
            break;

        // Игра началась, ход игрока
        case GameClientState::MY_TURN:
            // Спрятать кнопки
            this->enableButtons(false);

            // Состояние полей
            this->myField_->setState(GameField::FieldState::READY);
            this->enemyField_->setState(GameField::FieldState::ENEMY_READY);
            break;

        // Игра завершилась
        case GameClientState::ENDGAME_WIN:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Победа!");
            msgBox.setText("Вы победили!");
            msgBox.setIcon(QMessageBox::Icon::Information);
            msgBox.exec();
            break;
        }
        case GameClientState::ENDGAME_LOOSE:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Поражение.");
            msgBox.setText("Вы проиграли.");
            msgBox.setIcon(QMessageBox::Icon::Information);
            msgBox.exec();
            break;
        }
        case GameClientState::ENDGAME_DISCONNECTED:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Противник отключился.");
            msgBox.setText("Второй игрок покинул игру. Продолжение не возможно");
            msgBox.setIcon(QMessageBox::Icon::Information);
            msgBox.exec();
            break;
        }
    }
}

/// S L O T S

/**
 * Обработчик события нажатия на кнопку готовности к игре
 */
void GameWindow::onReadyButtonClicked()
{
    // Получить отправителя события (сигнала)
    auto button = qobject_cast<QPushButton*>(sender());

    // Если все корабли размещены
    if(myField_ != nullptr && myField_->allShipsPlaced())
    {
        this->gameStartWindow_->show();
    }
    // Если не все корабли размещены на поле
    else
    {
        // Сообщение
        QMessageBox msgBox;
        msgBox.setWindowTitle("Внимание!");
        msgBox.setText("Игру невозможно начать, пока все корабли не будут размещены на поле.");
        msgBox.setIcon(QMessageBox::Icon::Warning);
        msgBox.exec();
    }
}

/**
 * Обработчик события окна настроек подключения
 */
void GameWindow::onSettingsButtonClicked()
{
    // Обновить поля и показать окна
    this->settingsWindow_->refreshFields();
    this->settingsWindow_->show();
}

/**
 * Обработчик события готовности сокета к чтению данных
 */
void GameWindow::onReadyReadServerMessage()
{
    // Проверка на всякий случай (хотя казалось бы, нахуя? Но пусть будет..)
    if(_server != nullptr && _server->isConnected())
    {
        // Читаем сообщение
        net::Msg serverMessage = _server->readMessage();

        // В зависимости от типа
        switch(serverMessage.getType())
        {
            // Не опознанный тип сообщений
            case net::MSG_UNDEFINED:
            default:
                break;

            // Состояние игры
            case net::MSG_GAME_STATUS:
            {
                if(serverMessage.toMsgGameStatus().getStatus() == net::GAME_RUNNING){
                    this->currentState_ = GameClientState::WHOSE_TURN;
                }
                else if(serverMessage.toMsgGameStatus().getStatus() == net::GAME_OVER_WIN){
                    this->currentState_ = GameClientState::ENDGAME_WIN;
                }
                else if(serverMessage.toMsgGameStatus().getStatus() == net::GAME_OVER_LOOSE){
                    this->currentState_ = GameClientState::ENDGAME_LOOSE;
                }
                else{
                    this->currentState_ = GameClientState::ENDGAME_DISCONNECTED;
                }

                this->onStateChange();
                break;
            }

            // Доступность хода
            case net::MSG_SHOT_AVAILABLE:
            {
                if(serverMessage.toMsgShotAvailable().isAvailable()){
                    this->currentState_ = GameClientState::MY_TURN;
                }else{
                    this->currentState_ = GameClientState::ENEMY_TURN;
                }
                this->onStateChange();
                break;
            }

            // Результат хода игрока
            case net::MSG_SHOT_RESULTS:
            {
                auto shotResults = serverMessage.toMsgShotResults().getResults();
                if(shotResults == net::SHOT_RESULT_HIT){
                    enemyField_->shotAt(enemyField_->getLastShotCoordinates(),GameField::ShotType::HIT);
                }else if(shotResults == net::SHOT_RESULT_MISS){
                    enemyField_->shotAt(enemyField_->getLastShotCoordinates(),GameField::ShotType::MISS);
                }else{
                    enemyField_->shotAt(enemyField_->getLastShotCoordinates(),GameField::ShotType::DESTROYED);
                }
                break;
            }

            // Детали хода противника
            case net::MSG_SHOT_DETAILS:
            {
                // Координаты выстрела
                auto shotDetails = serverMessage.toMsgShotDetails().getDetails();
                // Координаты
                auto point = QPoint{static_cast<int>(shotDetails.x), static_cast<int>(shotDetails.y)};
                // Найти часть корабля по координатам
                auto shipPart = myField_->findAt(point);

                // Если такая часть найдена
                if(shipPart != nullptr){
                    // Отметить ее как уничтоженную
                    shipPart->isDestroyed = true;
                    // Если корабль, которому принадлежит часть уничтожен
                    if(shipPart->ship->isDestroyed()){
                        // Если все корабли на поле уничтожены - сообщить о победе
                        if(myField_->allShipsDestroyed()){
                            _server->sendMessage(net::MsgShotResults(net::SHOT_RESULT_WIN));
                        }
                        // Если не все корабли уничтожены, а только этот - сообщить об уничтожении
                        else{
                            _server->sendMessage(net::MsgShotResults(net::SHOT_RESULT_DESTROYED));
                        }
                    }
                    // Если корабль не уничтожен - обычное попадание
                    else{
                        _server->sendMessage(net::MsgShotResults(net::SHOT_RESULT_HIT));
                    }
                }
                // Если часть не найдена - отправить сообщение о промашке
                else{
                    // Отметить промашку на поле
                    myField_->shotAt(point,GameField::ShotType::MISS);
                    // Отправить сообщение
                    _server->sendMessage(net::MsgShotResults(net::SHOT_RESULT_MISS));
                }

                break;
            }
        }
    }
}
