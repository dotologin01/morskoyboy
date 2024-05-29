#include <QtWidgets>
#include "GameWindow.h"

#include "../NetworkApi/ServerPeer.hpp"

/// Настройки - IP сервера
QString _ip;
/// Настройки - Порт сервера
unsigned _port;
/// Объект для взаимодействия с сервером
net::ServerPeer* _server = nullptr;

/**
 * Точка входа
 * @param hInstance Дескриптор модуля Windows
 * @param hPrevInstance Не используется (устарело)
 * @param pCmdLine Аргументы запуска
 * @param nCmdShow Вид показа окна
 * @return Код выполнения
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Если ликовка QT статическая, нужно статически импортировать плагин
#ifdef QT_STATIC_BUILD
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

    // Инициализация QT
    int argc = 0;
    char* argv[] = {{}};
    QApplication app(argc, argv);

    // Настройки по умолчанию
    _ip = "127.0.0.1";
    _port = 1111;

    // Основное игровое окно
    QGraphicsScene scene(0, 0, 720, 630);
    GameWindow gameWindow(&scene);
    gameWindow.show();

    // Исполнение приложение и обработка всех событий
    return QApplication::exec();
}