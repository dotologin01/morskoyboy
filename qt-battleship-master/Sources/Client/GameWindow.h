#pragma once

#include <QtWidgets>

#include "GameField.h"
#include "SettingsWindow.h"
#include "GameStartWindow.h"

class GameWindow final : public QGraphicsView {
Q_OBJECT
public:
    /**
     * Инициализация игрового окна
     */
    explicit GameWindow(QGraphicsScene* scene);

    /**
     * Де-инициализация игрового окна
     */
    ~GameWindow() override;

    /**
     * Выстрел по вражескому полю
     * @param pos Положение клетки по которой осуществляется выстрел
     * @param gameField Указатель на поле
     * @param gameWindow Указатель на игровое окно (родительское для поля)
     */
    static void shotAtEnemy(const QPoint &pos, GameField* gameField, GameWindow* gameWindow);

    /**
     * Показать или скрыть кнопки
     * @param enable Показать
     */
    void enableButtons(bool enable = true);

    /**
     * Вызывается во время смены состояния игрового клиента
     */
    void onStateChange();

protected:
    /**
     * Переопределение события изменения размера
     */
    void resizeEvent(QResizeEvent *) override;

private slots:
    /**
     * Обработчик события нажатия на кнопку готовности к игре
     */
    void onReadyButtonClicked();

    /**
     * Обработчик события она настроек подключения
     */
    void onSettingsButtonClicked();

    /**
     * Обработчик события готовности сокета к чтению данных
     */
    void onReadyReadServerMessage();

private:
    /// Окно начала игры может менять состояние
    friend class GameStartWindow;

    /// Текущее состояние игрового клиента
    enum GameClientState
    {
        PREPARING,
        CONNECTED_NEW,
        CONNECTED_JOINED,
        WHOSE_TURN,
        MY_TURN,
        ENEMY_TURN,
        ENDGAME_WIN,
        ENDGAME_LOOSE,
        ENDGAME_DISCONNECTED
    } currentState_ = PREPARING;

    /// Окно настроек подключения
    SettingsWindow* settingsWindow_ = nullptr;
    /// Окно присоединения к игре
    GameStartWindow* gameStartWindow_ = nullptr;
    /// Игровое поле текущего игрока
    GameField* myField_ = nullptr;
    /// Игровое поле противника
    GameField* enemyField_ = nullptr;
    /// Label'ы для обозначения полей
    QVector<QLabel*> labels_;
    /// Кнопка готовности к игре
    QPushButton* btnReady_ = nullptr;
    /// Кнопка настроек подключения
    QPushButton* btnSettings_ = nullptr;
};