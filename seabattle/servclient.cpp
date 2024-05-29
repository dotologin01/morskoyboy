#include "servclient.h"

ServClient::ServClient(int sock, Server *parent): QObject(parent)
{
    _sock = sock;
    _serv = parent;
    byteAv = 0;
    for(int i=0; i<100; i++)
    {
        nowDamage[i]=0;
    }
    buf[0]=-1;
    int flags = 1;
    ioctl(_sock,FIONBIO,&flags); // опять же, чтобы не зависал I/O
}

void ServClient::readData(char* data, int byteCount) //считывание данных с сокета
{
    int bytesRead=0;
    bytesRead = recv(_sock, data, byteCount, 0); // принимаем сообщение от клиента
    if(bytesRead <= 0)
    {
        return;
    }
}

void ServClient::checkSock() //проверка доступных байтов
{
    if(byteAv||ioctl(_sock,FIONREAD,&byteAv)<0)
    {
        if(buf[0]==-1)
        {
            readData(buf,1);
            ioctl(_sock,FIONREAD,&byteAv);
        }
        int size;
        size = (buf[0]==comArrange)?100:1;
        if(byteAv<size)
        {
            ioctl(_sock,FIONREAD,&byteAv);
            return;
        }
        readData(&buf[1],size);
        switch(buf[0]){
        case comDot:
        {
            int cell = buf[1];
            _serv->sendShoot(_sock, cell);
        }
            break;
        case comArrange:
        {
            for(int i=0; i<100; i++)
            {
                field[i] = buf[i+1];
            }
            _serv->doStartGame();
        }
            break;
        default:
            break;
        }
        buf[0]=-1;
        ioctl(_sock,FIONREAD,&byteAv);
    }
}
char ServClient::getCell(int cell) //получение клетки
{
    return field[cell];
}
