#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <QObject>
#include <QTimer>
#include <sys/ioctl.h>
#include <string>
#include "mypoint.h"
#include "mainwindow.h"

enum com { comDot, //выстрел
           comArrange, //расположение
           comKill, //убит
           comDamage, //попадание
           comVoid, //промах
           comError, //нельзя стрелять
           comStartGame
        };
class MainWindow;
class Client: public QObject
{
    Q_OBJECT
public:
    Client(MainWindow *parent = 0);
    bool connectTo(char* hostinfo, int port); //подключение
    void writeData(char* data); //запись данных на сокет
    void readData(char* data, int byteCount); //считывание данных с сокета
    void sendArrange(QVector<int> &field); //отправить расположение
    void sendDot(char cell); //отправить выстрел

private:
    char buf[4];
    QTimer _timer;
    int _port;
    int sock;                 // дескриптор сокета
    struct sockaddr_in addr; // структура с адресом
    char* _hostinfo;
    int byteAv; //количество доступных байтов
    MainWindow* _parent;
    int win;
    int lose;
    bool blockSendDot;

private slots:
    void checkSock(); //проверка доступных байтов
};

#endif // CLIENT_H
