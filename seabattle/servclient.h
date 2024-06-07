#ifndef SERVCLIENT_H
#define SERVCLIENT_H
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
#include "client.h"
#include "server.h"
class Server;
class ServClient: public QObject
{
    Q_OBJECT
public:
    ServClient(int sock,Server* parent = 0);
    void writeData(char* data);
    void readData(char* data, int byteCount);
    char getCell(int cell);
    char nowDamage[100];

private:
    int _sock;
    char buf[100];
    char field[100];
    int byteAv;
    Server* _serv;

public slots:
    void checkSock();
};

#endif // SERVCLIENT_H
