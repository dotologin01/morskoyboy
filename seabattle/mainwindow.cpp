#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow *_mainWindow;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _mainWindow = this;
    isReady = false;
    isMyMove = false;
    _serv = new Server();
    _client = new Client(this);
    MyPoint::initPix();
    ui->placingBackVIew->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui->placingBackVIew->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    scene = new QGraphicsScene;
    //создание пустых полей
    itemField.resize(100);
    for(int x=0; x<10; x++)
    {
        for(int y=0; y<10; y++)
        {
            itemField[x+y*10] = new MyPoint(0);
            itemField[x+y*10]->setPos(367+x*28,112+y*28);
            itemField[x+y*10]->_coord=x+y*10;
            scene->addItem(itemField[x+y*10]);
        }
    }
    itemField2.resize(100);
    for(int x=0; x<10; x++)
    {
        for(int y=0; y<10; y++)
        {
            itemField2[x+y*10] = new MyPoint(0);
            itemField2[x+y*10]->setPos(50+x*28,112+y*28);
            itemField2[x+y*10]->_coord=-1;
            scene->addItem(itemField2[x+y*10]);
        }
    }
    //создание кораблей
    ships.resize(10);
    ships[0] = new MyPoint(4);
    ships[0]->setPos(20,100);
    scene->addItem(ships[0]);
    for (int i=0; i<2; i++)
    {
        ships[1+i] = new MyPoint(3);
        ships[1+i]->setPos(20+i*100, 155);
        scene->addItem(ships[1+i]);
    }
    for (int i=0; i<3; i++)
    {
        ships[3+i] = new MyPoint(2);
        ships[3+i]->setPos(20+i*80, 200);
        scene->addItem(ships[3+i]);
    }
    for (int i=0; i<4; i++)
    {
        ships[6+i] = new MyPoint(1);
        ships[6+i]->setPos(20+i*40, 250);
        scene->addItem(ships[6+i]);
    }
    for(int i=0; i<10; i++)
        ships[i]->hide();
    for(int i=0; i<100; i++)
        itemField[i]->hide();
    startGame();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::startGame()
{
    ui->stackedWidget->setCurrentIndex(0);
    scene->addPixmap(QPixmap(":/images/StartBack.png"));
    ui->startBackView->setScene(scene);
}
void MainWindow::placingShips()
{
    ui->stackedWidget->setCurrentIndex(1);
    scene->addPixmap(QPixmap(":/images/PlacingBack.png"));
    ui->placingBackVIew->setScene(scene);
    for(int i=0; i<10; i++)
    {
        ships[i]->show();
        ships[i]->setZValue(1);
    }
    for(int i=0; i<100; i++)
    {
        itemField[i]->show();
        itemField[i]->setZValue(1);
    }
}

void MainWindow::game()
{
    ui->stackedWidget->setCurrentIndex(2);
    scene->addPixmap(QPixmap(":/images/FieldBack.png"));
    ui->fieldBackView->setScene(scene);
    for(int j=0; j<=9; j++)
    {
        for(int i=0; i<=9; i++)
        {
            itemField2[i+j*10]->changeType(field[i+j*10]);
        }
        ships[j]->hide();
    }
    for(int i=0; i<100; i++)
    {
        itemField2[i]->show();
        itemField2[i]->setZValue(1);
    }
}
void MainWindow::setAroundShip(int xCell,int yCell,QVector<int>& aroundShip)
{
    if(yCell<=9 && yCell>=0 && xCell<=9 && xCell>=0)
        aroundShip[xCell+yCell*10]= 1;
}

bool MainWindow::checkShipsPlace(int numShip,int xCell, int yCell, QVector<int>& aroundShip)
{
        //проверка на правильность расстановки кораблей
        if ((xCell >= 0 && xCell <= 9) && (yCell >= 0 && yCell <= 9))
        {
            int typeShip = ships[numShip]->_typeShip;
            bool vert = ships[numShip]->isVertical;
            if(vert)
            {
                //вертикально клетки над кораблем
                setAroundShip(xCell-1,yCell-1,aroundShip); //сверху справа
                setAroundShip(xCell+1,yCell-1,aroundShip); //сверху слева
                setAroundShip(xCell,yCell-1,aroundShip); //сверху по центру
                //вертикально клетки под кораблем
                setAroundShip(xCell-1, yCell+typeShip, aroundShip); //аналогично снизу
                setAroundShip(xCell+1, yCell+typeShip, aroundShip);
                setAroundShip(xCell, yCell+typeShip, aroundShip);
            } else {
                //горизонтально слева
                setAroundShip(xCell-1,yCell-1,aroundShip); //сверху слева
                setAroundShip(xCell-1,yCell+1,aroundShip); //снизу слева
                setAroundShip(xCell-1,yCell,aroundShip); //слева от корабля
                //горизонтально справа
                setAroundShip(xCell+typeShip, yCell-1, aroundShip); //аналогично справа
                setAroundShip(xCell+typeShip, yCell+1, aroundShip);
                setAroundShip(xCell+typeShip, yCell, aroundShip);
            }
            for(int j = 0;j<typeShip;j++)
            {
                if(vert) //корабль вертикально
                {

                    if(aroundShip[xCell+(yCell+j)*10]==0)
                    {
                        setAroundShip(xCell-1,yCell+j,aroundShip); //слева
                        setAroundShip(xCell+1,yCell+j,aroundShip); //справа
                        setAroundShip(xCell,yCell+j,aroundShip); //центр
                        field[xCell+(yCell+j)*10]= MyPoint::CL_SHIP; //палуба
                    } else {
                        return false;
                    }
                } else { //горизонтально

                    if(aroundShip[(xCell+j)+yCell*10]==0)
                    {
                        setAroundShip(xCell+j,yCell,aroundShip); //центр
                        setAroundShip(xCell+j,yCell-1,aroundShip); //сверху
                        setAroundShip(xCell+j,yCell+1,aroundShip); //снизу
                        field[(xCell+j)+yCell*10]= MyPoint::CL_SHIP;
                    }
                    else {
                        return false;
                    }
                }
            }
        }
        else
            return false;
        return true;
}


 void MainWindow::on_startButton_clicked()
{
    if(isReady)
        return;


    qDebug()<<"This is aroundShip:";
    field.clear();
    field.resize(100);
    QVector<int> aroundShip;
    aroundShip.resize(100);
    for(int i=0; i<10; i++)
    {
        qreal x = ships[i]->x();
        qreal y = ships[i]->y();
        int xCell, yCell;
        xCell=(x-353)/28;
        yCell=(y-96)/28;
        //проверка на правильность расстановки кораблей
        if(!checkShipsPlace(i,xCell,yCell,aroundShip))
            return;
    }
    isReady = true;
        qDebug()<<"This is your Field:";
        for(int j=0; j<=9; j++)
        {
            QString s;
            for(int i=0; i<=9; i++)
            {
                s+= QString::number(field[i+j*10]) + " ";
            }
            qDebug() << s;
        }
    _client->sendArrange(field);

}

void MainWindow::on_New_game_clicked()
{
    char hostinfo[16]="127.0.0.1";
    _serv->doStartServer(3634);
    if(_client->connectTo(hostinfo,3634))
    {
        placingShips();
    }
}

void MainWindow::on_Connect_clicked()
{
    char hostinfo[16];
    QByteArray ba = ui->lineEdit->text().toLatin1();
    strcpy(hostinfo,ba.data());
    if(_client->connectTo(hostinfo,3634))
    {
        placingShips();
    }
}

void MainWindow::shoot(char cell) //выстрел
{
    if(cell!=-1 && isMyMove)
    {
        _client->sendDot(cell);
    }
}
void MainWindow::changeCellType(char cell,int type)
{
    itemField[cell]->changeType(type);
}

void MainWindow::changeCellType2(char cell,int type)
{
    itemField2[cell]->changeType(type);
}
void MainWindow::setStatus(QString str)
{
    ui->lineEdit_2->setText(str);
}
