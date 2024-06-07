#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QString>
#include <QVector>
#include <QDebug>
#include <mypoint.h>
#include <QMessageBox>
#include "client.h"
#include "server.h"

namespace Ui {
    class MainWindow;
}
class Client;
class Server;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString getField();
    QString getMyField();
    QString getEnemyField();
    void startGame();
    void placingShips();
    void game();
    void shoot(char cell);
    void changeCellType(char cell,int type);
    void changeCellType2(char cell,int type);
    void setStatus(QString str);
    bool isMyMove;
public slots:

private slots:
    void on_startButton_clicked();
    void on_New_game_clicked();
    void on_Connect_clicked();
private:
    Ui::MainWindow *ui;
    QGraphicsScene  *scene;
    MyPoint *point;
    QVector<MyPoint*> itemField;
    QVector<MyPoint*> itemField2;
    QVector<MyPoint*> ships;
    QVector<int> field; 
    Client* _client;
    Server* _serv;
    bool isReady;
    bool checkShipsPlace(int numShip, int xCell, int yCell, QVector<int>& aroundShip);
    void setAroundShip(int xCell, int yCell, QVector<int>& aroundShip);
};


#endif // MAINWINDOW_H
