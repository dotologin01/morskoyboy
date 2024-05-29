#pragma once

#include <QWidget>

// Класс для взаимодействия с UI интерфейсом
QT_BEGIN_NAMESPACE
namespace Ui { class GameStartWindow; }
QT_END_NAMESPACE

class GameWindow;
class GameStartWindow final : public QWidget
{
Q_OBJECT

public:
    /**
     * Конструктор окна
     * @param parent Родительский элемент
     */
    explicit GameStartWindow(GameWindow *mainWindow);

    /**
     * Деструктор окна
     */
    ~GameStartWindow() override;

private slots:
    /**
     * Обработка события нажатия на кнопку создания новой сессии
     */
    void on_btnNewSession_clicked();

    /**
     * Обработка события нажатия на кнопку подключения к сессии
     */
    void on_btnJoin_clicked();

private:
    /// Указатель на главное окно игры
    GameWindow* gameWindow_;

    /// Указатель на UI объект
    Ui::GameStartWindow* ui_;

};
