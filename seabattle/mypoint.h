#ifndef MYPOINT_H
#define MYPOINT_H
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>


class MyPoint : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    static const quint8 CL_CLEAR=0;
    static const quint8 CL_SHIP=1;
    static const quint8 CL_DOT=2;
    static const quint8 CL_HALF=3;

    explicit MyPoint(int typeShip, QObject *parent = 0);
    ~MyPoint();
    QPixmap* ship;
    int _width;
    int _height;
    int _typeShip;
    char _coord;
    static QPixmap* dot;
    static QPixmap* cell;
    static QPixmap* half;
    static QPixmap* kill;
    static QPixmap* sh_1;
    static QPixmap* sh_2;
    static QPixmap* sh_3;
    static QPixmap* sh_4;
    static void initPix();
    bool isVertical=false;
    void changeType(int type);

signals:
    void signal1();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
private:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // MYPOINT_H
