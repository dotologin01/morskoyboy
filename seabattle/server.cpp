#include "server.h"
Server::Server()
{
    srand(time(0));
    readyToPlay = false;
}
bool Server::doStartServer(qint16 port) //запуск сервера
{
    _listener = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int flags = 1;
    ioctl(_listener,FIONBIO,&flags);
    int yes = 1;
    setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); //для того, чтобы можно было сразу после выключения сервера
                                                                        //создать его снова на том же адресе

    /* SO_REUSEADDR - Указывает, что правила проверки адресов, передаваемых с помощью вызова bind,
     *  должны позволять повторное использование локального адреса. В случае с сокетами PF_INET это означает,
     *  что сокет может быть подключен, но не в тех случаях, когда активному слушающему сокету присвоен адрес.
     *  Если существует слушающий сокет, подключенный к определенному порту, с адресом INADDR_ANY,
     *  то к этому порту невозможно будет подключится с любого локального адреса.
     */
    if (_listener == -1)
    {
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof (stSockAddr));

    stSockAddr.sin_family = PF_INET;
    stSockAddr.sin_port = htons(port);
    stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_listener, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr)) == -1)
    {
        perror("Error: bind");
        close(_listener);
    }
    if (listen(_listener, 10) == -1)
    {
        perror("Error: listen");
        qDebug() << "Server not started at" << "127.0.0.1" << ":" << port;
        close(_listener);
        return false;
    }
    qDebug() << "Server started at" << "127.0.0.1" << ":" << port;
    QObject::connect(&_timer,SIGNAL(timeout()),this,SLOT(checkSock()));
    _timer.start(500);
    return true;
}
void Server::checkSock() //проверка новых соединений клиентов
{
    static int k=0;
    fd_set readfds; // слежение за операциями чтения
    struct timeval timeout;
    FD_ZERO(&readfds); //очищение изначального множества
    FD_SET(_listener, &readfds);
    timeout.tv_sec = 0; // ждать не более 100 нс
    timeout.tv_usec = 100;
    int sel = select(_listener + 1 , &readfds , NULL , NULL , &timeout); //добавление нового клиента
    if(sel < 0)
    {
        return;
    }
    if(!(FD_ISSET(_listener, &readfds)))
    {
        return;
    }
    qDebug()<<"accept";
    numsock[k] = accept(_listener, NULL, NULL); // принимаем входные подключение и создаем отделный сокет для каждого нового подключившегося клиента
    clientSock[k] = new ServClient(numsock[k],this);
    QObject::connect(&_timer,SIGNAL(timeout()),clientSock[k],SLOT(checkSock()));
    if(_listener < 0)
    {
        qDebug()<<"fail";
        return;
    } else {
        qDebug()<<"succesfull";
    }
    k++;
    if(k==2){
        return;
    }
}
bool Server::doStartGame() // начало игры
{
    if(!readyToPlay)
    {
        readyToPlay = true; //первый игрок вызовет эту функцию и будет ждать второго игрока
        return false;
    }
    //игра начнется после готовности второго игрока
    char data[2];
    data[0] = comStartGame;
    int r = rand()%2;
    data[1] = r;               //"рулетка" между игроками
    send(numsock[0], data, 2, 0);
    data[1] = 1-r;
    send(numsock[1], data, 2, 0);
    return true;
}
int Server::getEnemyCell(int sock, int cell) //получение поля соперника
{
    if(sock==numsock[0]) {
        return clientSock[1]->getCell(cell);
    } else {
        return clientSock[0]->getCell(cell);
    }
}
ServClient* Server::getEnemyServClient(int sock)
{
    if(sock==numsock[0])
    {
        return clientSock[1];
    } else {
        return clientSock[0];
    }
}

int Server::getEnemySocket(int sock)
{
    if(sock==numsock[0])
    {
        return numsock[1];
    } else {
        return numsock[0];
    }
}
int Server::checkDamageShips(int x, int y, int sock){
    ServClient *enemy = getEnemyServClient(sock);
    if(x<=9 && x>=0 && y<=9 && y>=0 && getEnemyCell(sock,x+y*10)) //клетки справа
    {
        if(!(enemy->nowDamage[x+y*10]==1)){
            return 1; //Если палуба есть, но не подбита
        } else {
            return 2; //Если палуба есть и подбита
        }
    }
    return 0; //Если палубы нет
}
bool Server::checkDamageCell(int a, int b, int x, int y,int &iKill, int nowKilled[], int sock){
    for(int i=1;i<=3;i++) {
        int dmg = checkDamageShips(x+i*a,y+i*b,sock);
        if(dmg == 2) {
            nowKilled[iKill] = x+i*a+(y+i*b)*10;//записываем координаты подбитых палуб
            iKill++;
        } else {
            if(dmg==1)
                return false;
            break;
        }
    }
    return true;
}

bool Server::checkShipsPlace(int x, int y, int sock, int nowKilled[])
{
    int iKill=1;
    bool isKill = true;
    nowKilled[0] = x+y*10;
    for(int i =1;i<4;i++)
    {
        nowKilled[i] = -1; //если рядом клеток нет
    }
    //проверяем на наличие палуб и на ранение
    isKill=checkDamageCell(1, 0, x, y, iKill, nowKilled, sock) &&
           checkDamageCell(-1, 0, x, y, iKill, nowKilled, sock) &&
           checkDamageCell(0, 1, x, y, iKill, nowKilled, sock) &&
           checkDamageCell(0, -1, x, y, iKill, nowKilled, sock);
    return isKill;
}

void Server::sendShoot(int sock, int cell)
{
    char data[2];
    int nowKilled[4];
    ServClient *enemy = getEnemyServClient(sock);
    int enemySock = getEnemySocket(sock);
    if(enemy->nowDamage[cell]==0)
    {
        if(getEnemyCell(sock,cell)==0) // проверка получунного поля соперника
        {
            data[0] = comVoid;
            data[1] = cell;
            send(sock, data, 2, 0);
            send(enemySock, data, 2, 0);

        } else {
            //расшифровка cell
            int x = cell%10; //единица в cell (остаток от деления)
            int y = cell/10; //десятка в cell

            //проверяем на наличие палуб и на ранение

            if(checkShipsPlace(x, y, sock, nowKilled))
            {
                char dataKill[5]; //передаем координаты убитого корабля
                dataKill[0] = comKill;
                for(int i=0;i<4;i++)
                {
                    dataKill[i+1]=nowKilled[i];
                }
                send(sock, dataKill, 5, 0); //отправляем координаты
                send(enemySock, dataKill, 5, 0); //обоим игрокам
            } else { //записываем коор-ты клетки в которые стреляли обоим игрокам
                data[0] = comDamage;
                data[1] = cell;
                send(sock, data, 2, 0);
                send(enemySock, data, 2, 0);
            }
        }
        enemy->nowDamage[cell] = 1;
    } else {
        data[0] = comError;
        send(sock, data, 2, 0);
    }

}
