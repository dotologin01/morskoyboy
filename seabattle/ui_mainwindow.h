/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *StartMenu;
    QGraphicsView *startBackView;
    QLineEdit *lineEdit;
    QPushButton *New_game;
    QPushButton *Connect;
    QWidget *Placing;
    QGraphicsView *placingBackVIew;
    QPushButton *startButton;
    QWidget *Game;
    QGraphicsView *fieldBackView;
    QLineEdit *lineEdit_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(642, 422);
        MainWindow->setMinimumSize(QSize(642, 422));
        MainWindow->setMaximumSize(QSize(642, 444));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setMinimumSize(QSize(642, 422));
        centralWidget->setMaximumSize(QSize(642, 422));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, 0, 642, 422));
        stackedWidget->setMinimumSize(QSize(642, 422));
        stackedWidget->setMaximumSize(QSize(642, 422));
        StartMenu = new QWidget();
        StartMenu->setObjectName(QStringLiteral("StartMenu"));
        StartMenu->setMinimumSize(QSize(642, 422));
        StartMenu->setMaximumSize(QSize(642, 422));
        StartMenu->setBaseSize(QSize(640, 420));
        StartMenu->setAutoFillBackground(false);
        startBackView = new QGraphicsView(StartMenu);
        startBackView->setObjectName(QStringLiteral("startBackView"));
        startBackView->setGeometry(QRect(0, 0, 642, 422));
        startBackView->setMinimumSize(QSize(642, 422));
        startBackView->setMaximumSize(QSize(642, 422));
        startBackView->setBaseSize(QSize(640, 420));
        startBackView->setAutoFillBackground(true);
        startBackView->setInteractive(false);
        lineEdit = new QLineEdit(StartMenu);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(200, 220, 251, 51));
        QFont font;
        font.setPointSize(18);
        lineEdit->setFont(font);
        lineEdit->setLayoutDirection(Qt::LeftToRight);
        lineEdit->setAlignment(Qt::AlignCenter);
        New_game = new QPushButton(StartMenu);
        New_game->setObjectName(QStringLiteral("New_game"));
        New_game->setGeometry(QRect(250, 140, 161, 71));
        Connect = new QPushButton(StartMenu);
        Connect->setObjectName(QStringLiteral("Connect"));
        Connect->setGeometry(QRect(250, 280, 161, 71));
        stackedWidget->addWidget(StartMenu);
        Placing = new QWidget();
        Placing->setObjectName(QStringLiteral("Placing"));
        Placing->setEnabled(true);
        Placing->setMinimumSize(QSize(642, 422));
        Placing->setMaximumSize(QSize(642, 422));
        placingBackVIew = new QGraphicsView(Placing);
        placingBackVIew->setObjectName(QStringLiteral("placingBackVIew"));
        placingBackVIew->setGeometry(QRect(0, 0, 642, 422));
        placingBackVIew->setMinimumSize(QSize(642, 422));
        placingBackVIew->setMaximumSize(QSize(642, 422));
        startButton = new QPushButton(Placing);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(50, 300, 181, 71));
        stackedWidget->addWidget(Placing);
        Game = new QWidget();
        Game->setObjectName(QStringLiteral("Game"));
        Game->setMinimumSize(QSize(642, 422));
        Game->setMaximumSize(QSize(642, 422));
        fieldBackView = new QGraphicsView(Game);
        fieldBackView->setObjectName(QStringLiteral("fieldBackView"));
        fieldBackView->setGeometry(QRect(0, 0, 642, 422));
        fieldBackView->setMinimumSize(QSize(642, 422));
        fieldBackView->setMaximumSize(QSize(642, 422));
        fieldBackView->setAutoFillBackground(false);
        lineEdit_2 = new QLineEdit(Game);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(130, 10, 201, 21));
        stackedWidget->addWidget(Game);
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        lineEdit->setText(QApplication::translate("MainWindow", "127.0.0.1", Q_NULLPTR));
        New_game->setText(QApplication::translate("MainWindow", "NEW GAME", Q_NULLPTR));
        Connect->setText(QApplication::translate("MainWindow", "CONNECT", Q_NULLPTR));
        startButton->setText(QApplication::translate("MainWindow", "START", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
