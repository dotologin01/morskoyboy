#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <string>
#include "servclient.h"
#include <ctime>
class ServClient;
class Server: public QObject
{
    Q_OBJECT
public:
    bool doStartServer(qint16 port);
    bool doStartGame();
    int getEnemyCell(int sock,int cell);
    void sendShoot(int sock, int cell);
    Server();

private:
    QTimer _timer;
    struct sockaddr_in stSockAddr;
    int numsock[2];
    ServClient* clientSock[2];
    int _listener;
    bool readyToPlay;
    ServClient* getEnemyServClient(int sock); //узнать о противнике
    int getEnemySocket(int sock); //узнать о противнике
    int checkDamageShips(int x, int y, int sock); //проверить повреждение кораблей
    bool checkShipsPlace(int x, int y, int sock, int nowKilled[]); //проверить расположение кораблей
    bool checkDamageCell(int a, int b, int x, int y,int &iKill, int nowKilled[], int sock); //проверить повреждение клетки
private slots:
    void checkSock();
};

#endif // SERVER_H
