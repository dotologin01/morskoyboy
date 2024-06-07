#include "client.h"
extern MainWindow* _mainWindow;
Client::Client(MainWindow *parent) : QObject(parent)
{
    byteAv = 0; //количество доступных байтов
    _parent = parent;
    win = 0;
    lose = 0;
    buf[0]=-1; //для подготовки к приему данных
    blockSendDot = false;
}
bool Client::connectTo(char* hostinfo,int port){
    sock = socket(AF_INET, SOCK_STREAM, 0); // создание TCP-сокета
    if(sock < 0)
    {
        perror("socket");
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // Указываем параметры сервера; // или любой другой порт...
    addr.sin_addr.s_addr = inet_addr(hostinfo);
    _port = port;
    _hostinfo = hostinfo;
    if(::connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) // установка соединения с сервером
    {
        perror("Подключение");
        return false;
    }
    int flags = 1;  //для того, чтобы не было зависаний
    ioctl(sock,FIONBIO,&flags); // типа для того, чтобы не блокировался I/O
    QObject::connect(&_timer,SIGNAL(timeout()),this,SLOT(checkSock()));
    _timer.start(100); //проверка по таймеру каждые 100 мс
    return true;
    close(sock);
}

void Client::writeData(char* data) //запись данных на сокет
{
    int bytesWrite = sizeof(data);
    send(sock, data, bytesWrite,0); //в порядке поступления байтов
}

void Client::readData(char* data, int byteCount) //считывание данных с сокета
{
    int bytesRead = 0;
    bytesRead = recv(sock, data, byteCount,0); // принимаем сообщение с сокета
    if(bytesRead <= 0)
    {
        return; //проверка на получение данных
    }
}

void Client::checkSock() //проверка доступных байтов
{
    if(byteAv || ioctl(sock,FIONREAD,&byteAv)<0) //если данные не получены
    {
        if(buf[0]==-1)
        {
            readData(buf,1); //считываем первый байт
            ioctl(sock,FIONREAD,&byteAv); //обновляем данные о доступных байтов
        }
        int size; //определяем размер оставшихся байт в блоке
        size = (buf[0]==comKill)?4:1; //и записываем
                                //для координат палуб
        if(byteAv < size)
        {
            ioctl(sock,FIONREAD,&byteAv);
            return;
        } //если данные получены
        readData(&buf[1],size); //то читаем все оставшиеся данные в блоке
        blockSendDot = false;
        switch(buf[0]){
        case comVoid: //промах
            if(_parent->isMyMove)
            {
                _mainWindow->changeCellType(buf[1],5);
                _mainWindow->setStatus("enemyMove");
            } else {
                _mainWindow->changeCellType2(buf[1],5);
                _mainWindow->setStatus("yourMove");
            }
            _parent->isMyMove = !_parent->isMyMove;  //передаем ход другому игроку
            break;
        case comDamage:
            if(_parent->isMyMove)
            {
              _mainWindow->changeCellType(buf[1],6);
            } else {
              _mainWindow->changeCellType2(buf[1],6);
            }
            break;
        case comKill:
            if(_parent->isMyMove)
            {
                win++;
                for(int i=0; i<4; i++)
                {
                    if(buf[i+1]!=-1)
                    {
                        _mainWindow->changeCellType(buf[i+1],7);
                    }
                }
            } else {
                lose++;
                for(int i=0; i<4; i++)
                {
                    if(buf[i+1]!=-1)
                    {
                        _mainWindow->changeCellType2(buf[i+1],7);
                    }
                }
            }
            if(win==10)
            {
                _mainWindow->setStatus("youWin");
                _parent->isMyMove=false;
            }
            if(lose==10)
            {
                _mainWindow->setStatus("youLose");
            }
            break;
        case comStartGame:
            _parent->game();
            if(buf[1])
            {
                _parent->isMyMove = true;
                _mainWindow->setStatus("yourMove");
            } else {
                _mainWindow->setStatus("enemyMove");
            }
            break;
        case comError:
            break;
        default:
            break;
        }
        ioctl(sock,FIONREAD,&byteAv);
        buf[0]=-1;
    }
}

void Client::sendArrange(QVector<int>& field) // отправить расположение
{
    char data[101];
    data[0] = comArrange;
    for(int i = 1; i<=100; i++)
    {
        data[i] = field[i-1];
    }
    send(sock,data, 101,0);
}

void Client::sendDot(char cell) //отправить выстрел
{
    if(!blockSendDot)
    {
        char data[2];
        data[0] = comDot;
        data[1] = cell;
        send(sock,data, 2,0);
        blockSendDot = true; //для того, чтобы нельзя было стрелять дважды в одну клетку
    }
}
