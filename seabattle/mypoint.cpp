#include "mypoint.h"
#include "mainwindow.h"
QPixmap* MyPoint::dot;
QPixmap* MyPoint::cell;
QPixmap* MyPoint::half;
QPixmap* MyPoint::kill;
QPixmap* MyPoint::sh_1;
QPixmap* MyPoint::sh_2;
QPixmap* MyPoint::sh_3;
QPixmap* MyPoint::sh_4;

extern MainWindow* _mainWindow; // для вызова выстрела из MainWindow
MyPoint::MyPoint(int typeShip,QObject *parent) : QObject(parent), QGraphicsItem()
{
    _typeShip = typeShip;
    if(_typeShip!=0)
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    switch (_typeShip) {
    case 0:
        ship=cell;
        _width=28;
        _height=28;
        break;
    case 1:
        ship=sh_1;
        _width=28;
        _height=28;
        break;
    case 2:
        ship=sh_2;
        _width=58;
        _height=28;
        break;
    case 3:
        ship=sh_3;
        _width=84;
        _height=28;
        break;
    case 4:
        ship=sh_4;
        _width=112;
        _height=28;
        break;
    default:
        break;
    }
}
MyPoint::~MyPoint()
{
}
QRectF MyPoint::boundingRect() const
{
    /* возвращаем координаты расположения точки
       по ним будет определяться нажатие точки*/
    return QRectF(-14,-14,_width,_height);
}
void MyPoint::initPix(){
    dot = new QPixmap(":/images/Dot.png");
    cell = new QPixmap(":/images/cell.png");
    half = new QPixmap(":/images/Half.png");
    kill = new QPixmap(":/images/Kill.png");
    sh_1 = new QPixmap(":/images/sh_1.png");
    sh_2 = new QPixmap(":/images/sh_2.png");
    sh_3 = new QPixmap(":/images/sh_3.png");
    sh_4 = new QPixmap(":/images/sh_4.png");
}
void MyPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Устанавливаем кисть в QPainter и отрисовываем круг, то есть ТОЧКУ
    painter->setBrush(Qt::black);
    painter->drawPixmap(-14, -14, *ship, 0, 0, _width,_height);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void MyPoint::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signal1();
    if(_typeShip==0)
    {
       _mainWindow->shoot(_coord);
    }
    // Вызываем родительскую функцию события нажатия кнопки мыши
    QGraphicsItem::mousePressEvent(event);
}

void MyPoint::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){
    if (isVertical==false)
    {
        setRotation(90);
        isVertical = true;
    } else {
        setRotation(0);
        isVertical = false;
    }
    QGraphicsItem::mousePressEvent(event);
}

void MyPoint::changeType(int typeShip) 
{
    _typeShip = typeShip;
    switch (_typeShip) {
    case 0:
        ship=cell;
        _width=28;
        _height=28;
        break;
    case 1:
        ship=sh_1;
        _width=28;
        _height=28;
        break;
    case 2:
        ship=sh_2;
        _width=58;
        _height=28;
        break;
    case 3:
        ship=sh_3;
        _width=84;
        _height=28;
        break;
    case 4:
        ship=sh_4;
        _width=112;
        _height=28;
        break;
    case 5:
        ship=dot;
        _width=28;
        _height=28;
        break;
    case 6:
        ship=half;
        _width=28;
        _height=28;
        break;
    case 7:
        ship=kill;
        _width=28;
        _height=28;
        break;
    default:
        break;
    }
    update();
}

